#ifndef UTILITIES_CAIRO_CIRCLE_HPP
#define UTILITIES_CAIRO_CIRCLE_HPP

#include "cairo.defs.hpp"

namespace Utilities::Cairo
{
    struct Circle
    {
        v2d Position;
        double Radius;

        Circle() noexcept = default;
        Circle(double x, double y, double radius) noexcept : Position(v2d{x,y}), Radius(radius) {}
        Circle(v2d position, double radius) noexcept : Position(position), Radius(radius) {}

        inline void operator()(RefPtr<Context> const& context, scale_type scale = 1) const
        {
            auto const p = fixup_point(Position * scale);
            context->begin_new_sub_path();
            context->arc(VECTOR2_EXPAND(p), Radius, 0, 2 * M_PI);
        }

        inline void fill(RefPtr<Context> const& context, scale_type scale = 1) const
        {
            operator()(context, scale);
            context->fill();
        }
        inline void stroke(RefPtr<Context> const& context, double thickness, scale_type scale = 1) const
        {
            context->set_line_width(thickness);
            operator()(context, scale);
            context->stroke();
        }
        inline void paint(RefPtr<Context> const& context, scale_type scale = 1) const
        {
            operator()(context, scale);
            context->paint();
        }
    };
}

#endif // UTILITIES_CAIRO_CIRCLE_HPP
