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

#endif //UTILITIES_MACRO_HPP