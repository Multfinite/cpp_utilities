#ifndef UTILITIES_OBTAINABLE_ITEM_HPP
#define UTILITIES_OBTAINABLE_ITEM_HPP

#include "item_lock.hpp"

namespace Utilities
{
	template<typename T>
	struct obtainable_item
	{
	public:
	private:
		T _item;
		std::mutex _lock;
	public:
		obtainable_item() = default;

		template<typename ...TArgs>
		obtainable_item(TArgs... args) : _item(...args) {}

		item_lock<T> obtain() { return item_lock<T> { _item, _lock }; }
		T& direct() { return _item; }
	};
}

#endif //UTILITIES_OBTAINABLE_ITEM_HPP