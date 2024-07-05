#ifndef UTILITIES_ALGORITHM_HPP
#define UTILITIES_ALGORITHM_HPP

#include <algorithm>

#include "exceptions.hpp"

namespace Utilities
{
        template<
            typename ForwardIt1, typename ForwardIt2,
            typename UnaryOp, typename Predicate
        >
        ForwardIt2 transform_if(
            ForwardIt1 first1, ForwardIt1 last1,
            ForwardIt2 d_first, UnaryOp unary_op,
            Predicate predicate
        ) {
            for (; first1 != last1; ++d_first, ++first1)
                if(predicate(*first1))
                    *d_first = unary_op(*first1);

            return d_first;
        }

        template<typename TContainer>
        auto indexof(const TContainer& container, const typename TContainer::value_type& _Val) ->
                decltype(container.begin(), container.end())
        {
                size_t index = 0;
                for(auto it = container.cbegin()
                    ; it != container.cend()
                    ; ++it, ++index)
                {
                    if(*it == _Val)
                        return index;
                }
                throw construct_error_no_msg(Exceptions::item_not_found_exception);
        }
        template<typename TContainer, typename TPredicate>
        auto indexof_if(const TContainer& container, const TPredicate& pred, bool raise = true) ->
                decltype(container.begin(), container.end())
        {
                size_t index = 0;
                for(auto it = container.cbegin()
                    ; it != container.cend()
                    ; ++it, ++index)
                {
                    if(pred(*it))
                        return index;
                }
                throw construct_error_no_msg(Exceptions::item_not_found_exception);
        }

        template<typename TContainer>
        auto find(const TContainer& container, const typename TContainer::value_type& _Val, bool raise = true) ->
                decltype(container.begin(), container.end())
        {
                auto iterator = std::find(container.begin(), container.end(), _Val);
                if (raise && iterator == container.end())
                        throw construct_error_no_msg(Exceptions::item_not_found_exception);
                return iterator;
        }
        template<typename TContainer, typename TPredicate>
        auto find_if(const TContainer& container, const TPredicate& pred, bool raise = true) ->
                decltype(container.begin(), container.end())
        {
                auto iterator = std::find_if(container.begin(), container.end(), pred);
                if (raise && iterator == container.end())
                        throw construct_error_no_msg(Exceptions::item_not_found_exception);
                return iterator;
        }

        template<typename InputIt, typename LinkedIt, typename T>
        auto find_linked(InputIt cb, InputIt ce, LinkedIt lb, const T& _Val)
        {
            while(cb != ce)
            {
                if(*cb == _Val) break;
                ++cb; ++ lb;
            }
            return lb;
        }

        template<typename TContainer, typename TLinkedContainer, typename T>
        auto find_linked(const TContainer& container, const TLinkedContainer& linked, const T& _Val, bool raise = true)
            -> decltype (linked.begin(), linked.end())
        {
            auto li = find_linked(container.begin(), container.end(), linked.begin(), _Val);
            if (raise && li == linked.end())
                throw construct_error_no_msg(Exceptions::item_not_found_exception);
            return li;
        }

        template<typename InputIt, typename LinkedIt, typename TPredicate>
        auto find_linked_if(InputIt cb, InputIt ce, LinkedIt lb, const TPredicate& pred)
        {
            while(cb != ce)
            {
                if(pred(*cb)) break;
                ++cb; ++lb;
            }
            return lb;
        }

        template<typename TContainer, typename TLinkedContainer, typename TPredicate>
        auto find_linked_if(const TContainer& container, const TLinkedContainer& linked, const TPredicate& pred, bool raise = true)
            -> decltype (linked.begin(), linked.end())
        {
            auto li = find_linked_if(container.begin(), container.end(), linked.begin(), pred);
            if (raise && li == linked.end())
                throw construct_error_no_msg(Exceptions::item_not_found_exception);
            return li;
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

        template<typename TDestinationContainer, typename TContainer>
        void assign(TDestinationContainer& dst, const TContainer& other)
        {
            dst.assign(other.cbegin(), other.cend());
        }

        /*!
         * @brief Clone (deep copy) object. Must be specialized before - it's just interface.
         * @arg o: object-source
         * @arg c: object-destination
         */
        template<typename T>
        inline void clone(const T& o, T& c);
}

#endif // UTILITIES_ALGORITHM_HPP
