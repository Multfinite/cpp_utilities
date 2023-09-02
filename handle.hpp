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

namespace Utilities
{
	template<typename T, typename D>
	std::unique_ptr<T, D> make_unique(T t, D d)
	{
		return std::unique_ptr<T, D>(t, d);
	}
}

#endif //UTILITIES_HANDLE_HPP