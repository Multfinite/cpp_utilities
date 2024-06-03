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

template<typename T>
inline T& reference_cast(T* ptr) { return (T&)*ptr; /* reinterpret_cast<T&>(ptr); */ }

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
	requires Clearable<T>
#endif
inline void clear(const T& ...args)
{
	for (auto& x : {args...})
		x.clear();
}

#endif //UTILITIES_MACRO_HPP
