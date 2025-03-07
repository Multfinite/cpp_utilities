#ifndef UTILITIES_CAIRO_RECT_HPP
#define UTILITIES_CAIRO_RECT_HPP

#include "cairo.defs.hpp"

namespace Utilities::Cairo
{
    struct Rect
    {
        v2d Position, Size;

        inline v2d bottom_left() const { return Position; }
        inline v2d bottom_right() const { return Position + v2d { Size.X, 0 }; }
        inline v2d top_left() const { return Position + v2d { 0, Size.Y }; }
        inline v2d top_right() const { return Position + Size; }
        inline v2d center() const { return Position + Size / 2; }

        Rect() = default;
        Rect(v2d position, v2d size) : Position(position), Size(size) {}
        inline static Rect from_points(v2d bottomLeft, v2d topRight) { return Rect(bottomLeft, topRight - bottomLeft); }

        inline void operator()(RefPtr<Context> const& context, scale_type scale) const noexcept
        {
            auto const pos = fixup_point(Position * scale);
            auto const sz = fixup_point(Size * scale);

            context->rectangle(VECTOR2_EXPAND(pos), VECTOR2_EXPAND(sz));
        }

        inline void fill(RefPtr<Context> const& context, scale_type scale) const noexcept
        {
            operator()(context, scale);
            context->fill();
        }
        inline void stroke(RefPtr<Context> const& context, double thickness, scale_type scale) const noexcept
        {
            context->set_line_width(thickness);
            operator()(context, scale);
            context->stroke();
        }
        inline void paint(RefPtr<Context> const& context, scale_type scale) const noexcept
        {
            operator()(context, scale);
            context->paint();
        }
    };
}

#endif // UTILITIES_CAIRO_RECT_HPP
