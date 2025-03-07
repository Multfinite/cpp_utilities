#ifndef UTILITIES_CAIRO_POLYGON_HPP
#define UTILITIES_CAIRO_POLYGON_HPP

#include "cairo.defs.hpp"
#include "line.hpp"

namespace Utilities::Cairo
{
    struct Polygon
    {
        std::list<v2d> Points;

        Polygon() = default;
        Polygon(std::initializer_list<v2d> points) : Points(points) {}
        Polygon(std::initializer_list<Line> lines)
        {
            for(Line const& line : lines)
                for(v2d const& p : line.Points)
                    Points.push_back(p);
        }
        Polygon(std::initializer_list<Line*> lines)
        {
            for(Line const* line : lines)
                for(v2d const& p : line->Points)
                    Points.push_back(p);
        }
        /*!
         * @brief Создаёт полигон из двух линий.
         */
        Polygon(Line const& a, Line const& b)
        {
            for(v2d const& p : a.Points)
                Points.push_back(p);
            for(auto it = b.Points.rbegin(); it != b.Points.rend(); ++it)
                Points.push_back(*it);
        }

        inline void assign(std::initializer_list<v2d> points)
        {
             for (v2d const& p : points)
                Points.push_back(p);
        }
        inline void assign(std::initializer_list<Line> lines)
        {
            for(Line const& line : lines)
                for(v2d const& p : line.Points)
                    Points.push_back(p);
        }
        inline void assign(std::initializer_list<Line*> lines)
        {
            for(Line const* line : lines)
                for(v2d const& p : line->Points)
                    Points.push_back(p);
        }

        inline void operator()(RefPtr<Context> const& context, scale_type scale = 1) const
        {
            if(Points.size() < 3)
                throw construct_error(Exceptions::invalid_state_error, "Need at least 3 points.");

            context->begin_new_path();

            auto first = fixup_point(Points.front() * scale);
            context->move_to(VECTOR2_EXPAND(first));
            for(auto it = ++Points.begin(); it != Points.end(); ++it)
            {
                auto p = fixup_point(*it * scale);
                context->line_to(VECTOR2_EXPAND(p));
            }
            context->close_path();
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

#endif // UTILITIES_CAIRO_POLYGON_HPP
