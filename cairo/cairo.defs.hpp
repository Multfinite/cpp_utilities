#ifndef UTILITIES_CAIRO_CAIRO_DEFS_HPP
#define UTILITIES_CAIRO_CAIRO_DEFS_HPP

#include <glib.h>
#include <cairomm/cairomm.h>

#include "../math.hpp"
#include "../algorithm.hpp"
#include "../macro.hpp"
#include "../string.hpp"
#include "../optional.hpp"

namespace Utilities::Cairo
{
    struct rasterization_error : public Exceptions::base_error
    {
        rasterization_error(const std::string& function, const std::string& file, int line) : base_error("Rasterization failed.", function, file, line) { }
        rasterization_error(const std::string& msg, const std::string& function, const std::string& file, int line) : base_error(msg, function, file, line) { }

        optional<GError> Error;
        rasterization_error(const std::string& function, const std::string& file, int line, GError* pError) : base_error("Rasterization failed.", function, file, line), Error(*pError) { }
        rasterization_error(const std::string& msg, const std::string& function, const std::string& file, int line, GError* pError) : base_error(msg, function, file, line), Error(*pError) { }
    };

#if CAIROMM_MAJOR_VERSION == 1 && CAIROMM_MINOR_VERSION == 14
    using Format = ::Cairo::Format;
#define FORMAT(VALUE) Format::FORMAT_##VALUE
#elif CAIROMM_MAJOR_VERSION == 1 && CAIROMM_MINOR_VERSION == 16
    using Format = Cairo::Surface::Format;
#define FORMAT(value) Format::value
#else
    static_assert (FALSE, "CAIROMM unsupported version.")
#endif

    using ::Cairo::Surface;
    using ::Cairo::ImageSurface;

    using ::Cairo::Context;
    using ::Cairo::RefPtr;

    using v2d = Utilities::Math::Vector2<double>;
    using scale_type = double;

    struct Color
    {
        double R = 0, G = 0, B = 0, A = 0;

        Color() = default;
        Color(double r, double g, double b) : R(r), G(g), B(b), A(1) {}
        Color(double r, double g, double b, double a) : R(r), G(g), B(b), A(a) {}

        inline std::string as_hex() const noexcept
        {
            auto const m = static_cast<double>(std::numeric_limits<uint8_t>::max());
            uint8_t const r = std::ceil(R * m);
            uint8_t const g = std::ceil(G * m);
            uint8_t const b = std::ceil(B * m);
            return Utilities::string_format("%02x%02x%02x", r, g, b);
        }
        inline std::string as_hexa() const noexcept
        {
            auto const m = static_cast<double>(std::numeric_limits<uint8_t>::max());
            uint8_t const r = std::ceil(R * m);
            uint8_t const g = std::ceil(G * m);
            uint8_t const b = std::ceil(B * m);
            uint8_t const a = std::ceil(A * m);
            return Utilities::string_format("%02x%02x%02x%02x", r, g, b, a);
        }
        inline static Color from_hex(std::string hex)
        {
            std::string sr, sg, sb;
            auto srgb = { &sr, &sg, &sb };
            for(std::string* s : srgb)
                for(size_t i = 0; i < 2; ++i)
                {
                    s->push_back(hex.front());
                    hex.erase(hex.begin()); // hex.pop_front();
                }

            Color color { 0, 0, 0, 1 };
            auto crgb = { &color.R, &color.G, &color.B };
            int values[3] { 0, 0, 0 };

            auto iv = &values[0]; auto ic = crgb.begin();
            for(auto is = srgb.begin(); is != srgb.end(); ++is, ++ic, ++iv)
            {
                *iv = std::stoi(**is, nullptr, 0x10);
                **ic = *iv / static_cast<double>(std::numeric_limits<uint8_t>::max());
            }
            return color;
        }

        inline static Color compound(Color const& a, Color const& b) noexcept { return Color { a.R * b.R, a.G * b.G, a.B * b.B, a.A * b.A }; }
    };

    #define COLOR_EXPAND(color) color.R, color.G, color.B, color.A


    /*!
      @brief Документация по Cairo рекомендует добавить смещение чтобы линии были чёткими.
      @brief Связано это со способом растеризации самой библиотеки.
    */
    static constexpr v2d PointOffset = { 0.5, 0.5 };
    inline static v2d fixup_point(v2d const& point) noexcept
    {
        auto const xf = fraction_inlined(point.X);
        auto const yf = fraction_inlined(point.Y);
        return PointOffset + v2d {
            (xf < PointOffset.X) ? std::floor(point.X) : std::ceil(point.X)
          , (yf < PointOffset.Y) ? std::floor(point.Y) : std::ceil(point.Y)
        };
    }

    inline __fastcall v2d size_of(RefPtr<ImageSurface> const& image) noexcept { return v2d { static_cast<double>(image->get_width()), static_cast<double>(image->get_height()) }; }

    inline __fastcall void fill(RefPtr<Context> context, v2d size, Color color) noexcept
    {
        auto op = context->get_operator();
        context->set_operator(::Cairo::OPERATOR_CLEAR);
        context->set_source_rgba(COLOR_EXPAND(color));
        context->rectangle(VECTOR2_EXPAND(v2d::Zero()), VECTOR2_EXPAND(size));
        context->fill();
        context->set_operator(op);
    }
    inline __fastcall void clear_context(RefPtr<Context> context, v2d size) noexcept { fill(context, size, Color { 0, 0, 0, 0 }); };

    inline __fastcall void draw(RefPtr<Context> context, RefPtr<ImageSurface> image, v2d position = v2d::Zero(), double scale = 1) noexcept
    {
        auto const pos = fixup_point(position * scale);
        context->set_source(image, VECTOR2_EXPAND(pos));
        context->paint();
    }

    inline __fastcall void draw(RefPtr<Context> context, std::initializer_list<RefPtr<ImageSurface>> surfaces
        , v2d position, double width, double gap = 0, double scale = 1
    ) noexcept {
        v2d const size = v2d { 0, std::max(surfaces.size() - 1, size_t(0)) * gap }
                + std::accumulate(iterator_expand(surfaces), v2d::Zero(), [&](v2d a, RefPtr<ImageSurface> const& x) -> v2d { return a + size_of(x); });

        v2d offset { 0, 0 };
        for(RefPtr<ImageSurface> const& surface : surfaces)
        {
            auto const ssz = size_of(surface);
            auto const p = fixup_point((position + offset + v2d { std::max(0.0, width - ssz.X) / 2.0, 0 }) * scale);
            context->begin_new_sub_path();
            context->set_source(surface, VECTOR2_EXPAND(p));
            context->paint();
            offset = offset + v2d { 0, ssz.Y + gap };
        }
    }
}

#endif // UTILITIES_CAIRO_CAIRO_DEFS_HPP
