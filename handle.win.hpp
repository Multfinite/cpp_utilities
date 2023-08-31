#ifndef UTILITIES_HANDLE_WIN_HPP
#define UTILITIES_HANDLE_WIN_HPP

#include "handle.hpp"

#include <Windows.h>

template <>
struct ::std::default_delete<HMODULE> 
{
	default_delete()	= default;
	template <class U>
	constexpr default_delete(default_delete<U>) noexcept {}
	void operator()(HMODULE* p) const noexcept { FreeLibrary(*p); }
};

template <>
struct ::std::default_delete<HLOCAL> 
{
	default_delete() = default;
	template <class U>
	constexpr default_delete(default_delete<U>) noexcept {}
	void operator()(HLOCAL* p) const noexcept { LocalFree(*p); }
};

#endif //UTILITIES_HANDLE_WIN_HPP