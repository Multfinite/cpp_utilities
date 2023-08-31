#ifndef UTILITIES_HANDLE_HPP
#define UTILITIES_HANDLE_HPP

#include <memory>
#include <functional>

template <>
struct ::std::default_delete<FILE*>
{
	default_delete() = default;
	template <class U>
	constexpr default_delete(default_delete<U>) noexcept {}
	void operator()(FILE** p) const noexcept { fclose(*p); }
};

#endif //UTILITIES_HANDLE_HPP