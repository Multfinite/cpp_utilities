#ifndef UTILITIES_LINKING_NODE_HPP
#define UTILITIES_LINKING_NODE_HPP

#include "templates.hpp"

#include <vector>

namespace Utilities
{
    struct __linking_node
    {
        using linked_type = std::vector<void*>;

        linked_type Items;
        size_t ValidCount = 0;

        size_t next_index() const { return Items.size(); }
        size_t free_index() const
        {
            for (size_t i = 0; i <= Items.size(); i++)
            {
                if (!Items[i])
                    return i;
            }
            return Items.size();
        }
        void set(size_t index, linked_type::value_type ptr)
        {
            while (Items.size() <= index)
                Items.push_back(nullptr);
            Items[index] = ptr;
            ValidCount++;
        }
        void reset(size_t index)
        {
            while (Items.size() <= index)
                Items.push_back(nullptr);
            Items[index] = nullptr;
            ValidCount--;
        }
        void clear()
        {
            Items.clear();
            ValidCount = 0;
        }

        template<typename T>
        T& as_ref(size_t index) const { return (T&)Items[index]; }

        template<typename T>
        T* as_ptr(size_t index) const { return (T*)Items[index]; }
    };
#if HAS_CONCEPTS
    template<typename T>
    concept HasLinkingNode =
        std::is_same<__linking_node, decltype(T::__linking)>::value&&
        requires(T x)
    {
        x.__linking;
    };
#endif

    template<typename T>
#if HAS_CONCEPTS
        requires HasLinkingNode<T>
#endif
    struct __linked_node
    {
        using value_type = T;

        /*!
         * @brief This node attached to this object
         */
        value_type& Owner;
        /*!
         * @brief Index in linking node
         */
        size_t Index;

        __linked_node(value_type& owner, size_t index) : Owner(owner), Index(index) { link(); }
        __linked_node(value_type& owner) : Owner(owner), Index(owner.__linking.next_index()) { link(); }
        ~__linked_node() { unlink(); }

    private:
        void link()
        {
            Owner.__linking.set(Index, this);
        }
        void unlink()
        {
            Owner.__linking.reset(Index);
        }
    };
}
#endif // UTILITIES_LINKING_NODE_HPP
