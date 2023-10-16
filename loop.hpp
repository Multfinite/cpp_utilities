#ifndef UTILITIES_LOOP_HPP
#define UTILITIES_LOOP_HPP

#include "type_definitions.hpp"
#include "item_lock.hpp"
#include "obtainable_item.hpp"

#include <queue>
#include <chrono>
#include <thread>
#include <mutex>
#include <semaphore>

namespace Utilities::Threading
{
	using std::thread;
	using std::mutex;
	using std::binary_semaphore;
	using std::scoped_lock;

	using Utilities::item_lock;
	using Utilities::obtainable_item;

	template<typename TCallable, typename TContext, typename TItem>
	class Worker
	{
	public:
		using context_type = TContext;
		using callable_type = TCallable;
		using item_type = TItem;
		using item_queue_type = std::queue<item_type>;

		std::binary_semaphore InternalSemaphore;
	private:
		bool _terminate = false;
		bool _terminated = false;

		std::thread _thread;
		std::mutex _queueLock;
		std::mutex _contextLock;

		/* void callable(context_type& context, std::mutex& contextLock, item_queue_type& queue, std::mutex& queueLock); */
		callable_type _callable;
		context_type _context;
		item_queue_type _queue;

		void _loop()
		{
			while (true)
			{
				_queueLock.lock();
				if (_queue.empty())
				{
					_queue.unlock();
					InternalSemaphore.acquire();
				}

				if (_terminate)
				{
					_terminated = true;
					return;
				}

				_callable(_context, _contextLock, _queue, _queueLock);
			}
		}
	public:
		Worker() = default;
		Worker(callable_type callable, context_type context) : _callable(callable), _context(context) {}
		Worker(context_type context) : _context(context) {}
		Worker(callable_type callable) : _callable(callable) {}

		item_lock<item_queue_type> obtain_queue() { return item_lock<item_queue_type> { _queue, _queueLock }; }
		item_lock<context_type> obtain_context() { return item_lock<context_type>{ _context, _contextLock }; }

		item_lock<item_queue_type> try_obtain_queue() { return item_lock<item_queue_type> { _queue, __try_lock{}, _queueLock }; }
		item_lock<context_type> try_obtain_context() { return item_lock<context_type>{ _context, __try_lock{}, _contextLock }; }

		void run()
		{
			_thread = std::thread{ _loop };
		}
		void run_here()
		{
			_thread = std::thread{ _loop };
			_thread.join();
		}

		void terminate()
		{
			_terminate = true;
			InternalSemaphore.release();
		}
		bool is_terminated() const { return _terminated; }

		void wakeup()
		{
			InternalSemaphore.release();
		}
	};

	template<typename T, typename _shared_context_t>
	struct _worker_context : public obtainable_item<T>
	{
	public:
		_shared_context_t& SharedContext;

		_worker_context(_shared_context_t& shared) : SharedContext(shared), obtainable_item<T>() {}

		template<typename ...TArgs>
		_worker_context(_shared_context_t& shared, TArgs... args) : SharedContext(shared), obtainable_item<T>(...args) {}
	};
	/*!
	* @author multfinite
	*/
	template<
		typename TWorkerContext, typename TWorkerItem, typename TSharedContext, 
		typename TWorkerCallable, typename TMainCallable>
	class Loop
	{
	public:
		using shared_context = obtainable_item<TSharedContext>;
		using worker_context = _worker_context<TWorkerContext, TSharedContext>;
		using worker_type = Worker<TWorkerCallable, worker_context, TWorkerItem>;
	protected:
		vector<worker_type> _workers;
		shared_context _shared;

		/* void main(shared_context& shared, vector<worker_type>& workers); */
		TMainCallable _main;
	public:
		Loop(TMainCallable main, size_t workers, TWorkerCallable worker) :
			_main(main), _worker(worker)
		{
			_workers.reserve(workers);
			for (auto i = 0; i < workers; ++i)
			{
				auto& worker = _workers.emplace_back(_worker, worker_context{ _shared });
				worker.run();
			}
		}
		template<typename ...TArgs>
		Loop(TMainCallable main, size_t workers, TWorkerCallable worker, TArgs... workerContextArgs) :
			_main(main), _worker(worker)
		{
			_workers.reserve(workers);
			for (auto i = 0; i < workers; ++i)
			{
				auto& worker = _workers.emplace_back(_worker, worker_context{ _shared, ...workerContextArgs });
				worker.run();
			}
		}

		void run()
		{
			while (true)
			{
				_main(_shared, _workers);
			}
		}
	};
}

#endif //UTILITIES_LOOP_HPP