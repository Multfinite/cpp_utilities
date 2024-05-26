#ifndef UTILITIES_MACRO_HPP
#define UTILITIES_MACRO_HPP

template<typename T>
inline T& reference_cast(T* ptr) { return (T&)*ptr; /* reinterpret_cast<T&>(ptr); */ }

template<typename T>
inline T* offset_ptr(void* ptr, size_t offset = 0)
{
	unsigned char* pointer = static_cast<unsigned char*>(ptr) + offset;
	return reinterpret_cast<T*>(pointer);
}

#if __cplusplus >= 202002L
template<typename T>
concept Clearable = requires(T x) { x.clear(); };
#endif

template<typename ...T>
#if __cplusplus >= 202002L
	requires Clearable<T>
#endif
inline void clear(const T& ...args)
{
	for (auto& x : args)
		x.clear();
}

#endif //UTILITIES_MACRO_HPP