#ifndef UTILITIES_CAIRO_LINE_HPP
#define UTILITIES_CAIRO_LINE_HPP

#include "cairo.defs.hpp"

namespace Utilities::Cairo
{
    struct Line
    {
        std::list<v2d> Points;

        Line() = default;
        Line(v2d const& a, v2d const& b)
        {
            Points.push_back(a); Points.push_back(b);
        }
        Line(std::initializer_list<v2d> points) : Points(points) {}

        inline void operator()(RefPtr<Context> const& context, scale_type scale = 1, bool adjustExistingPath = false) const
        {
            if(Points.size() < 2)
                throw construct_error(Exceptions::invalid_state_error, "Need at least 2 points.");

            if(!adjustExistingPath) context->begin_new_sub_path();
    #if 0
            context->move_to(VECTOR2_EXPAND((*Points.begin() + PointOffset) * scale));
            for(auto it = ++Points.begin(); it != Points.end(); ++it)
                context->line_to(VECTOR2_EXPAND((*it + PointOffset) * scale));
    #else
            auto first = fixup_point(Points.front() * scale);
            context->move_to(VECTOR2_EXPAND(first));
            for(auto it = ++Points.begin(); it != Points.end(); ++it)
            {
                auto p = fixup_point(*it * scale);
                context->line_to(VECTOR2_EXPAND(p));
            }
    #endif
            context->stroke();
        }

        inline void fill(RefPtr<Context> const& context, scale_type scale = 1, bool adjustExistingPath = false) const
        {
            operator()(context, scale, adjustExistingPath);
            context->fill();
        }
        inline void stroke(RefPtr<Context> const& context, double thickness, scale_type scale = 1, bool adjustExistingPath = false) const
        {
            context->set_line_width(thickness);
            operator()(context, scale, adjustExistingPath);
            context->stroke();
        }
        inline void paint(RefPtr<Context> const& context, scale_type scale = 1, bool adjustExistingPath = false) const
        {
            operator()(context, scale, adjustExistingPath);
            context->paint();

        }

        inline void assign(std::initializer_list<v2d> points)
        {
             for (v2d const& p : points)
                Points.push_back(p);
        }

        inline friend std::ostream& operator<<(std::ostream& stream, Line const& line) noexcept
        {
            for(auto pIt = line.Points.begin(); pIt != line.Points.end(); ++pIt)
            {
                if(pIt != line.Points.begin())
                    stream << ", ";
                stream << *pIt;
            }
            return stream;
        }

        inline v2d max() const noexcept
        {
            v2d x { std::numeric_limits<v2d::value_type>::min(), std::numeric_limits<v2d::value_type>::min() };
            for(v2d const& p : Points)
                x = v2d::max(x, p);
            return x;
        }

        inline v2d min() const noexcept
        {
            v2d x { std::numeric_limits<v2d::value_type>::max(), std::numeric_limits<v2d::value_type>::max() };
            for(v2d const& p : Points)
                x = v2d::min(x, p);
            return x;
        }
    };
}

#endif // UTILITIES_CAIRO_LINE_HPP
