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
        using list = concurrent_list<T>;
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
        size_type size() const noexcept { return _l.size(); }
        size_type max_size() const noexcept { return _l.max_size(); }

        void clear() { _l.clear(); }

        iterator insert(const_iterator pos, const value_type& value) { return _l.insert(pos, value); }
        iterator insert(const_iterator pos, size_type count, const value_type& value) { return _l.insert(pos, count, value); }
        template<class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) { return _l.insert<InputIt>(pos, first, last); }

        iterator erase(iterator pos) { return _l.erase(pos); }
        iterator erase(iterator first, iterator last) { return _l.erase(first, last); }

        void push_back(const value_type& value) { _l.push_back(value); }
        void pop_back() { _l.pop_back(); }
        void push_front(const value_type& value) { _l.push_front(value); }
        void pop_front() { _l.pop_front(); }

        void resize(size_type count) { _l.resize(count); }
        void resize(size_type count, const value_type& value) { _l.resize(count, value); }

        void merge(list& other) { _l.merge(other._l); }
        template<class Compare>
        void merge(list& other, Compare comp) { _l.merge(other._l, comp); }

        void splice(const_iterator pos, list& other) { _l.splice(pos, other); }
        void splice(const_iterator pos, list& other, const_iterator it ) { _l.splice(pos, other, it); }
        void splice(const_iterator pos, list& other, const_iterator first, const_iterator last) { _l.splice(pos, other, first, last); }

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
        iterator insert(const_iterator pos, value_type&& value) { return _l.insert(pos, value); }
        iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) { return _l.insert(pos, ilist); }
        template<class... Args>
        iterator emplace(const_iterator pos, Args&&... args) { return _l.emplace(pos, args...); }
        template<class... Args>
        iterator emplace_back(Args&&... args) { return _l.emplace_back(args...); }
        template<class... Args>
        iterator emplace_front(Args&&... args) { return _l.emplace_front(args...); }
        iterator erase(const_iterator pos) { return _l.erase(pos); }
        iterator erase(const_iterator first, const_iterator last) { return _l.erase(first, last); }

        void push_back(value_type&& value) { _l.push_back(value); }
        void push_front(value_type&& value) { _l.push_front(value); }

        void merge(list&& other) { _l.merge(other._l); }
        template<class Compare>
        void merge(list&& other, Compare comp) { _l.merge(other._l, comp); }

        void splice(const_iterator pos, list&& other) { _l.splice(pos, other); }
        void splice(const_iterator pos, list&& other, const_iterator it ) { _l.splice(pos, other, it); }
        void splice(const_iterator pos, list&& other, const_iterator first, const_iterator last) { _l.splice(pos, other, first, last); }
#endif
#if __cplusplus >= 201703L
        void swap(list& other) noexcept { _l.swap(other._l); }
#else
        void swap(list& other) { _l.swap(other._l); }
#endif
#if __cplusplus >=202302L
        // assign_range
        // insert_range
        // append_range
#endif
    };
}

#endif // UTILITIES_CONCURRENT_LIST_HPP
