#ifndef UTILITIES_CAIRO_TEXT_HPP
#define UTILITIES_CAIRO_TEXT_HPP

#include "cairo.defs.hpp"

#include <pangomm.h>

namespace Utilities::Cairo
{
    struct Text
    {
        Pango::FontDescription Desc;
        std::string Value;
        v2d Position;

        Text() = default;
        Text(std::string const& font, size_t size) :

            Desc(Pango::FontDescription(font + " " + std::to_string(size)))
        {}

        inline v2d get_size() const noexcept
        {
            auto surface = ImageSurface::create(FORMAT(ARGB32), 1, 1);
            auto context = Context::create(surface);
            return get_size(context);
        }

        inline v2d get_size(RefPtr<Context> context) const noexcept
        {
            auto layout = Pango::Layout::create(context);
            layout->set_font_description(Desc);
            layout->set_markup(Value);
            Utilities::Math::Vector2<int> pixSz;
            layout->get_pixel_size(VECTOR2_EXPAND(pixSz));
            return v2d::static_cast_from<int>(pixSz);
        }

        inline void operator()(RefPtr<Context> context, scale_type scale) const noexcept
        {
            auto const pos = fixup_point(Position * scale);
            auto layout = Pango::Layout::create(context);
            context->begin_new_sub_path();
            context->move_to(VECTOR2_EXPAND(pos));
            layout->set_text(Value);
            layout->show_in_cairo_context(context);
        }

        inline static void multiline_centered(RefPtr<Context> context, std::initializer_list<std::reference_wrapper<Text>> texts
            , v2d position, double width, double gap = 0, double scale = 1
        ) noexcept {
            auto layout = Pango::Layout::create(context);
            v2d const size = v2d { 0, std::max(texts.size() - 1, size_t(0)) * gap }
                    + std::accumulate(iterator_expand(texts), v2d::Zero(), [&](v2d a, Text& x) -> v2d { return a + x.get_size(); });

            v2d offset { 0, 0 };
            for(Text& text : texts)
            {
                auto const tsz = text.get_size();
                auto const p = fixup_point((position + offset + v2d { std::max(0.0, width - tsz.X) / 2.0, 0 }) * scale);
                context->begin_new_sub_path();
                context->move_to(VECTOR2_EXPAND(p));
                layout->set_text(text.Value);
                layout->show_in_cairo_context(context);
                offset = offset + v2d { 0, tsz.Y + gap };
            }
        }
    };
}

#endif // UTILITIES_CAIRO_TEXT_HPP
