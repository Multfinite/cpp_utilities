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

        static constexpr vector_type Zero = vector_type { 0, 0 };

        value_type X, Y;

        vector_type operator+(vector_type const& other) const { return vector_type { X + other.X, Y + other.Y }; };
        vector_type operator-(vector_type const& other) const { return vector_type { X - other.X, Y - other.Y }; };
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
    };
}

#define VECTOR2_EXPAND(v) v.X, v.Y

#endif // UTILITIES_MATH_HPP
