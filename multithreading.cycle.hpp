#ifndef UTILITIES_MULTITHREADING_CYCLE_HPP
#define UTILITIES_MULTITHREADING_CYCLE_HPP

#include <cstdlib>
#include <thread>

namespace Utilities::Multithreading
{
	class Cycle final
	{
	public:
		struct events_t
		{
			Utilities::ObjectEvent<Cycle> Update, Initialize, DeInitialize;
			events_t(Cycle* object) : Update(object), Initialize(object), DeInitialize(object) {}
		} Event;

		std::mutex cvm;
		std::condition_variable cv;
	private:
		std::thread::id _threadId;
		bool _terminate = false;
		bool _terminated = false;
		bool _running = false;

		std::shared_ptr<std::thread> _thread;

		void _loop()
		{
			_threadId = std::this_thread::get_id();
			Event.Initialize();
			while (true)
			{
				std::unique_lock lk{ cvm };
				cv.wait(lk, [&] {
					return _running || _terminate;
					});

				if (_terminate)
				{
					_terminated = true;
					lk.unlock();
					cv.notify_one();
					return;
				}

				Event.Update();
				lk.unlock();
				cv.notify_one();
			}
			Event.DeInitialize();
		}
	public:
		auto thread_id() const { return _threadId; }
		void run()
		{
			_thread = std::make_shared<std::thread>(&_loop, this);
		}
		void run_here()
		{
			//_thread = make_shared<std::thread>(&self_type::_loop, this);
			//_thread->join();
			_loop();
		}

		void start()
		{
			_running = true;
			cv.notify_all();
		}
		void stop()
		{
			_running = false;
			cv.notify_all();
		}
		void terminate()
		{
			_terminate = true;
			cv.notify_all();
		}
		constexpr bool is_running() const noexcept { return _running; }
		constexpr bool is_terminated() const noexcept { return _terminated; }

		inline auto wait()
		{
			std::unique_lock lk{ cvm };
			cv.wait(lk, [&]{ return !_running || _terminated; });
			return lk;
		}
	};
}

#endif // UTILITIES_MULTITHREADING_CYCLE_HPP