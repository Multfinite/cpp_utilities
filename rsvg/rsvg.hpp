#ifndef UTILITIES_RSVG_RSVG_HPP
#define UTILITIES_RSVG_RSVG_HPP

#include <librsvg/rsvg.h>
#include <memory>

template<> struct std::default_delete<RsvgHandle*>
{
    default_delete() = default;
    template <class U>
    constexpr default_delete(default_delete<U>) noexcept {}
    void operator()(RsvgHandle** p) const noexcept { GError* err = nullptr; rsvg_handle_close(*p, &err); }
};

#endif // UTILITIES_RSVG_RSVG_HPP
