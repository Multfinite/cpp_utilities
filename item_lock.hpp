#ifndef UTILITIES_ITEM_LOCK_HPP
#define UTILITIES_ITEM_LOCK_HPP

#include <mutex>
#include <functional>

namespace Utilities
{
	using std::mutex;
	using std::list;
	using std::vector;
	using std::reference_wrapper;

	struct __try_lock {};
	template<typename T>
	struct item_lock 
	{
	public:
		
	private:
		T& _item;
		list<reference_wrapper<std::mutex>> _mutexes;
	public:
		T& item() { return _item; }
		item_lock(T& item) : _item(item) {}

		template<typename ...TMutexes>
		item_lock(T& item, TMutexes&... mutexes) :  _item(item) 
		{
			lock(mutexes...);
		}
		template<typename ...TMutexes>
		item_lock(T& item, __try_lock ty, TMutexes&... mutexes) : _item(item)
		{
			auto index = try_lock(mutexes...);
			if (index != -1)
				throw std::runtime_error{ std::to_string(index) };
		}
		~item_lock() 
		{
			unlock();
		}

		template<typename ...TMutexes>
		void lock(TMutexes&... mutexes)
		{
			std::lock(mutexes);
			_mutexes = { mutexes };
		}
		template<>
		void lock<std::mutex>(std::mutex& mutex)
		{
			mutex.lock();
			_mutexes = { mutex };
		}
		template<typename ...TMutexes>
		auto try_lock(TMutexes&... mutexes)
		{
			auto r = std::try_lock(mutexes);
			_mutexes = { mutexes };
			return r;
		}
		template<>
		auto try_lock<std::mutex>(std::mutex& mutex)
		{
			int r = mutex.try_lock() ? -1 : 0;
			_mutexes = { mutex };
			return r;
		}
		void unlock()
		{
			for (auto& m : _mutexes)
				m.get().unlock();
		}
	};
}

#endif //UTILITIES_ITEM_LOCK_HPP