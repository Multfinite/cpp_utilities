#ifndef UTILITIES_LOOP_HPP
#define UTILITIES_LOOP_HPP

#include "type_definitions.hpp"
#include "item_lock.hpp"
#include "obtainable_item.hpp"

#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <semaphore>

namespace Utilities::Threading
{
	using std::thread;
	using std::mutex;
	using std::unique_lock;
	using std::scoped_lock;
	using std::lock_guard;
	using std::condition_variable;
	using std::binary_semaphore;

	using Utilities::item_lock;
	using Utilities::obtainable_item;

	template<typename TContext, typename TItem>
	struct WorkerBody
	{
		using context_type = TContext;
		using item_type = TItem;
		// buffered_list should be swapped after all done
		//using item_queue_type = buffered_list<item_type>;
		using item_queue_type = std::list<item_type>;

		item_queue_type Queue;
		context_type Context;
		std::mutex Mutex;

		WorkerBody() = default;
		//WorkerBody(const WorkerBody& rhs) : Queue(rhs.Queue), Context(rhs.Context) {}
		template<typename ...TArgs> WorkerBody(TArgs&&... args) : Queue(), Context(args...) {}
	};

	template<typename TCallable, typename TWorkerBody>
	class Worker
	{
	public:
		using callable_type = TCallable;
		using body_type = TWorkerBody;
		using self_type = Worker<callable_type, body_type>;
		//using function_type = std::function<void(body_type& body)>;
		
		std::mutex cvm;
		std::condition_variable cv;
	private:
		thread::id _threadId;
		bool _terminate = false;
		bool _terminated = false;

		std::shared_ptr<std::thread> _thread;
		callable_type _callable;

		std::shared_ptr<body_type> _body;

		void _loop()
		{
			_threadId = std::this_thread::get_id();
			while (true)
			{				
				unique_lock lk { cvm };
				cv.wait(lk, [&] {
					return !_body.get()->Queue.empty() || _terminate;
				});

				if (_terminate)
				{
					_terminated = true;
					lk.unlock();
					cv.notify_one();
					return;
				}
				
				_callable(*_body.get());
				lk.unlock();
				cv.notify_one();
			}
		}
	public:
		Worker() = default;
		Worker(callable_type callable) :
			_callable(callable), _body(make_shared<body_type>())
		{}
		template<typename ...TArgs>
		Worker(callable_type callable, TArgs&&... args) :
			_callable(callable), _body(make_shared<body_type>(args...))
		{}
		Worker(self_type&& rhs) = delete;
		Worker(const self_type& rhs) = delete;
		/*
		Worker(self_type&& rhs) :
			_thread(rhs._thread), _callable(rhs._callable), _body(rhs._body)
		{}
		Worker(const self_type& rhs) :
			_thread(rhs._thread), _callable(rhs._callable), _body(rhs._body)
		{}
		*/

		body_type& body() { return *_body.get(); }
		item_lock<body_type> obtain_body() { return item_lock<body_type>{ *_body.get(), *_body.get()->Mutex }; }
		item_lock<body_type> try_obtain_body() { return item_lock<body_type>{ *_body.get(), __try_lock{}, *_body.get()->Mutex }; }

		auto thread_id() const { return _threadId;  }
		void run()
		{
			_thread = make_shared<std::thread>(&self_type::_loop, this);
		}
		void run_here()
		{
			//_thread = make_shared<std::thread>(&self_type::_loop, this);
			//_thread->join();
			_loop();
		}

		void terminate()
		{
			_terminate = true;
			cv.notify_all();
		}
		bool is_terminated() const { return _terminated; }

		inline auto wait()
		{
			unique_lock lk { cvm };
			cv.wait(lk, [&]
			{
				return _body.get()->Queue.empty() || _terminated;
			});
			return lk;
		}
	};

	template<typename T, typename _shared_context_t>
	struct _worker_context : public obtainable_item<T>
	{
	public:
		using self_type = _worker_context<T, _shared_context_t>;

		std::shared_ptr<_shared_context_t> SharedContext;

		_worker_context() = default;
		_worker_context(std::shared_ptr<_shared_context_t> shared) 
			: SharedContext(shared), obtainable_item<T>() 
		{}

		template<typename ...TArgs>
		_worker_context(std::shared_ptr<_shared_context_t> shared, TArgs&&... args) 
			: SharedContext(shared), obtainable_item<T>(args...) 
		{}

		_worker_context(self_type&& rhs) :
			SharedContext(rhs.SharedContext)//, obtainable_item<T>(rhs)
		{}
		_worker_context(const self_type& rhs) :
			SharedContext(rhs.SharedContext)//, obtainable_item<T>(rhs)
		{}
	};
	/*!
	* @author multfinite
	*/
	template<typename TMainCallable, typename TWorkerCallable,	typename TWorkerContext, typename TWorkerItem, typename TSharedContext>
	class Loop
	{
	public:
		using callable_type = TMainCallable;
		using shared_context = obtainable_item<TSharedContext>;
		using worker_context = _worker_context<TWorkerContext, shared_context>;
		using worker_body = WorkerBody<worker_context, TWorkerItem>;
		using worker_type = Worker<TWorkerCallable, worker_body>;
		using worker_type_ptr = std::shared_ptr<worker_type>;
		using worker_collection = vector<worker_type_ptr>;
		//using function_type = std::function<void(shared_context& shared, vector<worker_type>& workers)>;
	protected:
		worker_collection _workers;
		std::shared_ptr<shared_context> _shared;
		callable_type _callable;
	public:
		Loop(callable_type main, size_t workers, worker_type::callable_type worker) :
			_callable(main), 
			_shared(make_shared<shared_context>())
		{
			_workers.reserve(workers);
			for (auto i = 0; i < workers; ++i)
			{
				worker_type_ptr pWrk = make_shared<worker_type>(worker, _shared);
				_workers.push_back(pWrk);
				worker_type& w = *pWrk.get();
				w.run();
			}
		}
		template<typename ...TArgs>
		Loop(callable_type main, size_t workers, worker_type::callable_type worker, TArgs&&... workerContextArgs) :
			_callable(main), 
			_shared(make_shared<shared_context>())
		{
			_workers.reserve(workers);
			for (auto i = 0; i < workers; ++i)
			{
				worker_type_ptr pWrk = make_shared<worker_type>(worker, _shared, workerContextArgs...);
				_workers.push_back(pWrk);
				worker_type& w = *pWrk.get();
				w.run();
			}
		}

		void run()
		{
			auto threadId = std::this_thread::get_id();
			while (true)
			{
				for (auto& pw : _workers)
					pw.get()->cvm.lock();

				_callable(
					(shared_context &) *_shared.get(), 
					(worker_collection&) _workers
				);
				for (auto& pw : _workers)
					pw.get()->cvm.unlock();

				for (auto& pw : _workers)
				{
					auto& w = *pw.get();
					w.cv.notify_one();
				}
				
				for (auto& pw : _workers)
				{
					auto& w = *pw.get();
					auto lk = w.wait();
				}
			}
		}
	};
}

#endif //UTILITIES_LOOP_HPP