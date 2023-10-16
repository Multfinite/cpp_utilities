#ifndef UTILITIES_ITEM_LOCK_HPP
#define UTILITIES_ITEM_LOCK_HPP

#include <mutex>

namespace Utilities
{
	using std::mutex;

	struct __try_lock {};
	template<typename T>
	struct item_lock 
	{
	public:
		
	private:
		T& _item;
		vector<std::mutex&> _mutexes;
	public:
		T& item() { return _item; }
		item_lock(T& item) : _item(item) {}

		template<typename ...TMutexes>
		item_lock(T& item, TMutexes&... mutexes) :  _item(item) 
		{
			lock(mutexes);
		}
		template<typename ...TMutexes>
		item_lock(T& item, __try_lock ty, TMutexes&... mutexes) : _item(item)
		{
			auto index = try_lock(mutexes);
			if (index != -1)
				throw std::runtime_error{ std::to_string(index) };
		}
		~item_lock() 
		{
			unlock();
		}

		template<typename ...TMutexes>
		void lock(TMutexes&&... mutexes)
		{
			std::lock(...mutexes);
			_mutexes = { ...mutexes };
		}
		template<typename ...TMutexes>
		auto try_lock(TMutexes&&... mutexes)
		{
			auto r = std::try_lock(...mutexes);
			_mutexes = { ...mutexes };
			return r;
		}
		void unlock()
		{
			for (auto& m : _mutexes)
				m.unlock();
		}
	};
}

#endif //UTILITIES_ITEM_LOCK_HPP