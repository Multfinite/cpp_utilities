#ifndef UTILITIES_LINKING_NODE_HPP
#define UTILITIES_LINKING_NODE_HPP

#include "templates.hpp"

namespace Utilities
{
    struct __linking_node
    {
        using linked_type = std::vector<void*>;

        linked_type Items;
        size_t ValidCount = 0;

        size_t current_index() const { return Items.size() - 1; }
        size_t next_index() const { return Items.size(); }
        size_t free_index() const
        {
            for (size_t i = 0; i < Items.size(); i++)
            {
                if (!Items[i])
                    return i;
            }
            return Items.size();
        }
        void set(size_t index, linked_type::value_type ptr)
        {
            while (index >= Items.size())
                Items.push_back(nullptr);
            Items[index] = ptr;
            ValidCount++;
        }
        void reset(size_t index)
        {
            while (index >= Items.size())
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
}
#endif
#endif // UTILITIES_LINKING_NODE_HPP
