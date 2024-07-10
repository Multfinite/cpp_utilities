#ifndef UTILITIES_MATH_HPP
#define UTILITIES_MATH_HPP

#include <cmath>

namespace Utilities::Math
{
    template<typename TNumeric>
    struct Vector2
    {
        using value_type = TNumeric;
        using vector_type = Vector2<value_type>;

        inline static vector_type Zero() { return vector_type { 0, 0 }; };
        inline static vector_type Identity() { return vector_type { 1, 1 }; };
        inline static vector_type XAxis() { return vector_type { 1, 0 }; };
        inline static vector_type YAxis() { return vector_type { 0, 1 }; };

        value_type X, Y;

        vector_type operator+(vector_type const& other) const { return vector_type { X + other.X, Y + other.Y }; };
        vector_type operator-(vector_type const& other) const { return vector_type { X - other.X, Y - other.Y }; };
        vector_type operator-() const { return vector_type { -X, -Y }; }
        vector_type operator*(TNumeric scalar) const { return vector_type { X * scalar, Y * scalar }; };
        vector_type operator/(TNumeric scalar) const { return vector_type { X / scalar, Y / scalar }; };
        bool operator==(vector_type const& other) const { return X == other.X && Y == other.Y; };
        bool operator!=(vector_type const& other) const { return X == other.X && Y == other.Y; };

        vector_type ceil() const { return vector_type { std::ceil(X), std::ceil(Y) }; };
        vector_type round() const { return vector_type { std::round(X), std::round(Y) }; };
        vector_type floor() const { return vector_type { std::floor(X), std::floor(Y) }; };

        static vector_type max(vector_type const& a, vector_type const& b) { return vector_type { std::max(a.X, b.X), std::max(a.Y, b.Y) }; }
        static vector_type min(vector_type const& a, vector_type const& b) { return vector_type { std::min(a.X, b.X), std::min(a.Y, b.Y) }; }
        static vector_type abs(vector_type const& a, vector_type const& b) { return vector_type { std::abs(a.X, b.X), std::abs(a.Y, b.Y) }; }

        template<typename TValueTypeTo>
        inline static Vector2<TValueTypeTo> static_cast_to(vector_type const& v) { return Vector2<TValueTypeTo> { static_cast<TValueTypeTo>(v.X), static_cast<TValueTypeTo>(v.Y) }; }
        template<typename TValueTypeFrom>
        inline static vector_type static_cast_from(Vector2<TValueTypeFrom> const& v) { return vector_type { static_cast<value_type>(v.X), static_cast<value_type>(v.Y) }; }
    };
}

#define VECTOR2_EXPAND(v) (v).X, (v).Y

#endif // UTILITIES_MATH_HPP
