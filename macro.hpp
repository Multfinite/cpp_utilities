#ifndef UTILITIES_MACRO_HPP
#define UTILITIES_MACRO_HPP

/* https://stackoverflow.com/a/56483887 */

#define CPP98 199711L
#define CPP03 CPP98
#define CPP11 201103L
#define CPP14 201402L
#define CPP17 201703L
#define CPP20 202002L
#define CPP23 202302L

#define CPP_SINCE(x, H) x >= H
#define CPP_REMOVED(x, H) x < H
#define CPP_UNTIL(x, H) x < H
#define CPP_BETWEEN(x, L, H) x < H && x >= L

#include <cstdlib>
#include <type_traits>
#include <initializer_list>

template<typename T>
inline T& reference_cast(T* ptr)
{
    if(!ptr) throw "POINTER IS NULL";
    return *ptr;
}
template<typename T>
inline T& reference_cast(void* ptr)
{
    if(!ptr) throw "POINTER IS NULL";
    return *reinterpret_cast<T*>(ptr);
}
template<typename T>
inline T const& reference_cast(T const* ptr)
{
    if(!ptr) throw "POINTER IS NULL";
    return *ptr;
}
template<typename T>
inline T const& reference_cast(void const* ptr)
{
    if(!ptr) throw "POINTER IS NULL";
    return *reinterpret_cast<T const*>(ptr);
}

template<typename T>
inline T* offset_ptr(void* ptr, size_t offset = 0)
{
    unsigned char* pointer = static_cast<unsigned char*>(ptr) + offset;
    return reinterpret_cast<T*>(pointer);
}

#if CPP_SINCE(__cplusplus, CPP20)
template<typename T>
concept Clearable = requires(T x) { x.clear(); };
#endif

template<typename ...T>
#if  CPP_SINCE(__cplusplus, CPP20)
        requires (Clearable<T> && ...)
#endif
inline void clear(const T& ...args)
{
	for (auto& x : {args...})
		x.clear();
}

template<typename T>
inline bool is_any_of(T const& value, std::initializer_list<T> const& variants)
{
    for(T const& variant : variants)
        if(variant == value)
            return  true;
    return false;
}

template<typename T, typename ...Ts>
inline bool is_any_of(T const& value, Ts const& ...variants)
{
    ([&]{
        if(value == variants)
            return true;
    }, ...);
    return false;
}

template<typename TComparer, typename T, typename ...Ts>
inline bool is_any_of_if(TComparer const& comp, T const& value, Ts const& ...variants)
{
    ([&]{
        if(comp(value, variants))
            return true;
    }, ...);
    return false;
}

template<typename T, typename TComparer>
inline bool is_any_of_if(T const& value, std::initializer_list<T> const& variants, TComparer const& comp)
{
    for(T const& variant : variants)
        if(comp(variant, value))
            return  true;
    return false;
}

template <typename TIterator>
inline auto value_of_iterator(TIterator const& x, TIterator const& end)
{
    using T = typename TIterator::value_type;
    if constexpr(std::is_pointer_v<T>)
         return x != end ? *x : nullptr;
    else
         return x != end ? &*x : nullptr;
}

#define GETTER_NAME(name) name
#define SETTER_NAME(name) set_##name

#define GETTER_DECL(name, type) type GETTER_NAME(name)()
#define SETTER_DECL(name, type) void SETTER_NAME(name)(type value)

#define GETTER_IMPL(owner, name, type) type owner::GETTER_NAME(name)()
#define SETTER_IMPL(owner, name, type) void owner::SETTER_NAME(name)(type value)

#define GETTER_V(name, field) std::remove_reference_t<decltype(field)> GETTER_NAME(name)()
#define GETTER_R(name, field) std::remove_reference_t<decltype(field)>& GETTER_NAME(name)()
#define GETTER_RC(name, field) std::remove_reference_t<decltype(field)> const& GETTER_NAME(name)()

#define SETTER_V(name, field) void SETTER_NAME(name)(std::remove_reference_t<decltype(field)> value)
#define SETTER_R(name, field) void SETTER_NAME(name)(std::remove_reference_t<decltype(field)>& value)
#define SETTER_RC(name, field) void SETTER_NAME(name)(std::remove_reference_t<decltype(field)> const& value)

#define GETTER_V_DEFAULT(name, field) GETTER_V(name, field) const noexcept { return field; }
#define GETTER_R_DEFAULT(name, field) GETTER_R(name, field) const noexcept { return field; }
#define GETTER_RC_DEFAULT(name, field) GETTER_RC(name, field) const noexcept { return field; }

#define SETTER_V_DEFAULT(name, field) SETTER_V(name, field) noexcept { field = value; }
#define SETTER_R_DEFAULT(name, field) SETTER_R(name, field) noexcept { field = value; }
#define SETTER_RC_DEFAULT(name, field) SETTER_RC(name, field) noexcept { field = value; }

#define iterator_expand(range) (range).begin(), (range).end()
#define citerator_expand(range) (range).cbegin(), (range).cend()

#endif //UTILITIES_MACRO_HPP
