#ifndef UTILITIES_CONCURRENT_LIST_HPP
#define UTILITIES_CONCURRENT_LIST_HPP

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <list>

#include "macro.hpp"

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
        std::mutex& mutex() { return _m; }

        // lock
        // try_lock
        // unlock

        concurrent_list() = default;
        explicit concurrent_list(const allocator_type& alloc) : _l(alloc) {}
        template<class InputIt>
        concurrent_list(InputIt first, InputIt last, const allocator_type& alloc = allocator_type()) : _l(first, last, alloc) {}

        // operator=

        void assign(size_type count, const T& value) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.assign(count, value); };
        template<class InputIt>
        void assign(InputIt first, InputIt last) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.assign(first, last); };

        allocator_type get_allocator() const { return _l.get_allocator(); }

        reference front() { return _l.front(); }
        const_reference front() const { return _l.front(); }

        reference back() { return _l.back(); }
        const_reference back() const { return _l.back(); }

        iterator insert(const_iterator pos, const value_type& value) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.insert(pos, value); }
        iterator insert(const_iterator pos, size_type count, const value_type& value) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.insert(pos, count, value); }
        template<class InputIt>
        iterator insert(const_iterator pos, InputIt first, InputIt last) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.insert(pos, first, last); }

        iterator erase(iterator pos) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.erase(pos); }
        iterator erase(iterator first, iterator last) { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.erase(first, last); }

        void push_back(const value_type& value) { auto lk = std::unique_lock<std::mutex>(_m);
            _l.push_back(value); }
        void pop_back() { auto lk = std::unique_lock<std::mutex>(_m);
            _l.pop_back(); }
        void push_front(const value_type& value) { auto lk = std::unique_lock<std::mutex>(_m);
            _l.push_front(value); }
        void pop_front() { auto lk = std::unique_lock<std::mutex>(_m);
            _l.pop_front(); }

        void resize(size_type count) { auto lk = std::unique_lock<std::mutex>(_m);
            _l.resize(count); }
        void resize(size_type count, const value_type& value) { auto lk = std::unique_lock<std::mutex>(_m);
            _l.resize(count, value); }

        void merge(list& other) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
            _l.merge(other._l); }
        template<class Compare>
        void merge(list& other, Compare comp) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
            _l.merge(other._l, comp); }

        void splice(const_iterator pos, list& other) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
            _l.splice(pos, other); }
        void splice(const_iterator pos, list& other, const_iterator it ) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
            _l.splice(pos, other, it); }
        void splice(const_iterator pos, list& other, const_iterator first, const_iterator last) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
            _l.splice(pos, other, first, last); }

        // remove
        // remove_if

        void sort() { auto lk = std::unique_lock<std::mutex>(_m);
            _l.sort(); }
        template<class Compare>
        void sort(Compare comp) { auto lk = std::unique_lock<std::mutex>(_m);
            _l.sort(comp); }

#if CPP_BETWEEN(__cplusplus, CPP11, CPP14)
        explicit concurrent_list(size_type count, const value_type& value = value_type(), const allocator_type& alloc = allocator_type())
            : _l(count, value, alloc) {}
        explicit concurrent_list(size_type count) : _l(count) {};
#elif CPP_SINCE(__cplusplus, CPP14)
        concurrent_list(size_type count, const value_type& value = value_type(), const allocator_type& alloc = allocator_type())
            : _l(count, value, alloc) {}
        explicit concurrent_list(size_type count, const allocator_type alloc = allocator_type()) : _l(count, alloc) {};
#endif

#if CPP_UNTIL(__cplusplus, CPP11)
        bool empty() const { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.empty(); }
#elif CPP_BETWEEN(__cplusplus, CPP11, CPP23)
        bool empty() const noexcept { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.empty(); }
#elif CPP_SINCE(__cplusplus, CPP23)
        [[nodiscard]] bool empty() const noexcept { auto lk = std::unique_lock<std::mutex>(_m);
            return _l.empty(); }
#endif

#if CPP_UNTIL(__cplusplus, CPP17)
    void swap(list& other) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.swap(other._l); }
#elif CPP_SINCE(__cplusplus, CPP17)
    void swap(list& other) noexcept { auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.swap(other._l); }
#endif

#if CPP_UNTIL(__cplusplus, CPP20)
    void remove(const T& value) { auto lk = std::unique_lock<std::mutex>(_m);
        _l.remove(value); }
    template<class UnaryPredicate>
    void remove_if(const T& value, UnaryPredicate p) { auto lk = std::unique_lock<std::mutex>(_m);
        _l.remove(value, p); }

    void unique(){ auto lk = std::unique_lock<std::mutex>(_m);
        _l.unique(); }
    template<typename BinaryPredicate>
    void unique(BinaryPredicate p){ auto lk = std::unique_lock<std::mutex>(_m);
        _l.unique(p); }
#elif CPP_SINCE(__cplusplus, CPP20)
    size_type remove(const T& value) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.remove(value); }
    template<class UnaryPredicate>
    size_type remove_if(const T& value, UnaryPredicate p) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.remove(value, p); }

    size_type unique(){ auto lk = std::unique_lock<std::mutex>(_m);
        return _l.unique(); }
    template<typename BinaryPredicate>
    size_type unique(BinaryPredicate p){ auto lk = std::unique_lock<std::mutex>(_m);
        return _l.unique(p); }
#endif

#if CPP_UNTIL(__cplusplus, CPP11)
    void clear() { auto lk = std::unique_lock<std::mutex>(_m);
        _l.clear(); }

    iterator begin() { return _l.begin();  }
    const_iterator begin() const { return _l.begin(); }

    iterator end() { return _l.end();  }
    const_iterator end() const { return _l.end(); }

    iterator rbegin() { return _l.rbegin();  }
    const_iterator rbegin() const { return _l.rbegin(); }

    iterator rend() { return _l.rend();  }
    const_iterator rend() const { return _l.rend(); }

    size_type size() const { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.size(); }
    size_type max_size() const { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.max_size(); }

    void reverse() { auto lk = std::unique_lock<std::mutex>(_m);
        _l.reverse(); }
#elif CPP_SINCE(__cplusplus, CPP11)
    concurrent_list(std::initializer_list<value_type> init, const allocator_type& alloc = allocator_type()) : _l(init, alloc) {}

    void assign(std::initializer_list<T> ilist) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.insert(ilist); }

    void clear() noexcept { auto lk = std::unique_lock<std::mutex>(_m);
        _l.clear(); }

    iterator begin() noexcept { return _l.begin();  }
    const_iterator begin() const noexcept { return _l.begin(); }

    iterator end() noexcept { return _l.end();  }
    const_iterator end() const noexcept { return _l.end(); }

    iterator rbegin() noexcept { return _l.rbegin();  }
    const_iterator rbegin() const noexcept { return _l.rbegin(); }

    iterator rend() noexcept { return _l.rend();  }
    const_iterator rend() const noexcept { return _l.rend(); }

    const_iterator cbegin() const noexcept { return _l.cbegin(); };
    const_iterator cend() const noexcept { return _l.cend(); };

    const_iterator crbegin() const noexcept { return _l.crbegin(); };
    const_iterator crend() const noexcept { return _l.crend(); };

    size_type size() const noexcept { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.size(); }
    size_type max_size() const noexcept { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.max_size(); }

    iterator insert(const_iterator pos, value_type&& value) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.insert(pos, value); }
    iterator insert(const_iterator pos, std::initializer_list<value_type> ilist) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.insert(pos, ilist); }
    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.emplace(pos, args...); }
    template<class... Args>
    iterator emplace_back(Args&&... args) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.emplace_back(args...); }
    template<class... Args>
    iterator emplace_front(Args&&... args) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.emplace_front(args...); }
    iterator erase(const_iterator pos) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.erase(pos); }
    iterator erase(const_iterator first, const_iterator last) { auto lk = std::unique_lock<std::mutex>(_m);
        return _l.erase(first, last); }

    void push_back(value_type&& value) { auto lk = std::unique_lock<std::mutex>(_m);
        _l.push_back(value); }
    void push_front(value_type&& value) { auto lk = std::unique_lock<std::mutex>(_m);
        _l.push_front(value); }

    void merge(list&& other) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.merge(other._l); }
    template<class Compare>
    void merge(list&& other, Compare comp) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.merge(other._l, comp); }

    void splice(const_iterator pos, list&& other) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.splice(pos, other); }
    void splice(const_iterator pos, list&& other, const_iterator it ) {auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.splice(pos, other, it); }
    void splice(const_iterator pos, list&& other, const_iterator first, const_iterator last) { auto lk = std::unique_lock<std::mutex>(_m, other._m);
        _l.splice(pos, other, first, last); }

    void reverse() noexcept { auto lk = std::unique_lock<std::mutex>(_m);
        _l.reverse(); }
#endif

#if CPP_SINCE(__cplusplus, CPP23)
    // assign_range
    // insert_range
    // append_range

#endif
    };
}

#endif // UTILITIES_CONCURRENT_LIST_HPP
