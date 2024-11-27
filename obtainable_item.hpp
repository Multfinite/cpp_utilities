#ifndef UTILITIES_OBTAINABLE_ITEM_HPP
#define UTILITIES_OBTAINABLE_ITEM_HPP

#include "item_lock.hpp"

namespace Utilities
{
	/*!
	* @brief Monitor
	*/
	template<typename T>
	struct obtainable_item
	{
	public:
		using value_type = T;
	private:
		std::shared_ptr<value_type> _item;
		std::shared_ptr<std::mutex> _mutex;
	public:
		obtainable_item() : 
			_mutex(make_shared<std::mutex>()),
			_item(make_shared<value_type>()) 
		{}
		template<typename ...TArgs>
		obtainable_item(TArgs&&... args) :
			_mutex(make_shared<std::mutex>()),
			_item(make_shared<value_type>(args...))
		{}
		obtainable_item(obtainable_item<T>&& rhs) : 
			_item(rhs._item), _mutex(rhs._mutex)
		{}
		obtainable_item(const obtainable_item<T>& rhs) :
			_item(rhs._item), _mutex(rhs._mutex)
		{}

		constexpr item_lock<value_type> obtain() { return item_lock<value_type> { *_item.get(), *_mutex.get() }; }
		constexpr value_type& get_unsafe() { return *_item.get(); }
	};
}

#endif //UTILITIES_OBTAINABLE_ITEM_HPP