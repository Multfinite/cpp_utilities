#ifndef UTILITIES_CAIRO_GFXENTRY_HPP
#define UTILITIES_CAIRO_GFXENTRY_HPP

#include "cairo.defs.hpp"

namespace Utilities::Cairo
{
    struct GfxEntry
    {
        RefPtr<ImageSurface> Image;
        RefPtr<Context> Context;

        GfxEntry() = default;
        GfxEntry(v2d const& Size)
            : GfxEntry(Size.X, Size.Y) {}
        GfxEntry(double w, double h)
            : GfxEntry(static_cast<int>(std::ceil(w)), static_cast<int>(std::ceil(h))) {}
        GfxEntry(int w, int h) :
              Image(ImageSurface::create(FORMAT(ARGB32), w, h))
            , Context(Context::create(Image))
        {}

        inline void resize(v2d const& size) { resize(size.X, size.Y); }
        inline void resize(double w, double h) { resize(static_cast<int>(std::ceil(w)), static_cast<int>(std::ceil(h))); }
        inline void resize(int w, int h)
        {
            Image = ImageSurface::create(FORMAT(ARGB32), w, h);
            Context = Context::create(Image);
        }
        inline void clear() noexcept { Context.clear(); Image.clear(); }

        inline v2d size() const noexcept { return v2d { static_cast<double>(Image->get_width()), static_cast<double>(Image->get_height()) }; }

        operator bool() const noexcept { return bool(Image); }
    };
    using GfxCache = std::map<scale_type, GfxEntry>;
}

#endif // UTILITIES_CAIRO_GFXENTRY_HPP
