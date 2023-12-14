#ifndef UTILITIES_ALGORITHM_HPP
#define UTILITIES_ALGORITHM_HPP

#include <algorithm>

#include "exceptions.hpp"

namespace Utilities
{
	template<typename TContainer>
	auto find(const TContainer& container, const typename TContainer::value_type& _Val) ->
		decltype(container.begin(), container.end())
	{
		auto iterator = std::find(container.begin(), container.end(), _Val);
		if (iterator == container.end())
			throw construct_error_no_msg(Exceptions::item_not_found_exception);
		return iterator;
	}
	template<typename TContainer, typename TPredicate>
	auto find_if(const TContainer& container, const TPredicate& pred) ->
		decltype(container.begin(), container.end())
	{
		auto iterator = std::find_if(container.begin(), container.end(), pred);
		if (iterator == container.end())
			throw construct_error_no_msg(Exceptions::item_not_found_exception);
		return iterator;
	}
	
	template<typename TOutContainer, typename TInContainer, typename TConverter>
	auto as(const TInContainer& _In, const TConverter& _Func) ->
		decltype(_In.cbegin(), _In.cend(), _In.size(), TOutContainer{})
	{
		TOutContainer _Out;
		_Out.resize(_In.size());
		std::transform(_In.cbegin(), _In.cend(), _Out.begin(), _Func);
		return _Out;
	}

	template<typename TContainer>
	auto contains(const TContainer& container, const typename TContainer::value_type& _Val) ->
		decltype(container.begin(), container.end(), bool{})
	{
		auto iterator = std::find(container.begin(), container.end(), _Val);
		return iterator != container.end();
	}
	template<typename TContainer, typename TPredicate>
	auto contains(const TContainer& container, const TPredicate& pred) ->
		decltype(container.begin(), container.end(), bool{})
	{
		auto iterator = std::find_if(container.begin(), container.end(), pred);
		return iterator != container.end();
	}
}

#endif // UTILITIES_ALGORITHM_HPP