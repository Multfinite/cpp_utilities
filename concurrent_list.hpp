#ifndef UTILITIES_CONCURRENT_LIST_HPP
#define UTILITIES_CONCURRENT_LIST_HPP

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <list>

namespace Utilities {
    template<typename T, typename TAllocator = std::allocator<T>>
    class concurrent_list
    {
    public:
        using list_type = std::list<T>;
        using value_type = typename list_type::value_type;
        using allocator_type = typename list_type::allocator_type;
        using size_type = typename list_type::size_type;
        using difference_type = typename list_type::difference_type;
        using reference = typename list_type::reference;
        using const_reference = typename list_type::const_reference;
        using pointer = typename list_type::pointer;
        using const_pointer = typename list_type::const_pointer;
        using iterator = typename list_type::iterator;
        using const_iterator = typename list_type::const_iterator;
        using reverse_iterator = typename list_type::reverse_iterator;
        using reverse_const_iterator = typename list_type::reverse_const_iterator;
    private:
        list_type _l;
        std::mutex _m;
    public:
        list_type& inner() { return _l; }

        // lock
        // try_lock
        // unlock

        // operator=
        // assign


        allocator_type get_allocator() const { return _l.get_allocator(); }

        reference front() { return _l.front(); }
        const_reference front() const { return _l.front(); }

        reference back() { return _l.back(); }
        const_reference back() const { return _l.back(); }

        iterator begin() { return _l.begin();  }
        const_iterator begin() const { return _l.begin(); }

        iterator end() { return _l.end();  }
        const_iterator end() const { return _l.end(); }

        iterator rbegin() { return _l.rbegin();  }
        const_iterator rbegin() const { return _l.rbegin(); }

        iterator rend() { return _l.rend();  }
        const_iterator rend() const { return _l.rend(); }

        bool empty() const noexcept { return _l.empty(); }
        size_t size() const noexcept { return _l.size(); }
        size_t max_size() const noexcept { return _l.max_size(); }

        void clear() { _l.clear(); }

        // insert
        // erase
        // push_back
        // pop_back
        // push_front
        // pop_front

        // resize
        // swap

        // merge
        // splice
        // remove
        // remove_if
        // reverse
        // unique
        // sort

#if __cplusplus >=201103L
        const_iterator cbegin() const noexcept { return _l.cbegin(); };
        const_iterator cend() const noexcept { return _l.cend(); };
        const_iterator crbegin() const noexcept { return _l.crbegin(); };
        const_iterator crend() const noexcept { return _l.crend(); };
        // emplace
        // emplace_back
        // emplace_front
#endif
#if __cplusplus >=202302L
    // assign_range
        // insert_range
        // append_range
#endif
    }
1}


#endif // UTILITIES_CONCURRENT_LIST_HPP
