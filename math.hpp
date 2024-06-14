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

        value_type X, Y;

        vector_type operator+(vector_type const& other) const { return vector_type { X + other.X, Y + other.Y }; };
        vector_type operator-(vector_type const& other) const { return vector_type { X - other.X, Y - other.Y }; };
        vector_type operator*(TNumeric scalar) const { return vector_type { X * scalar, Y * scalar }; };
        vector_type operator/(TNumeric scalar) const { return vector_type { X / scalar, Y / scalar }; };

        vector_type max(vector_type const& other) const { return vector_type { std::max(X, other.X), std::max(Y, other.Y) }; }
        vector_type min(vector_type const& other) const { return vector_type { std::min(X, other.X), std::min(Y, other.Y) }; }
        vector_type abs(vector_type const& other) const { return vector_type { std::abs(X, other.X), std::abs(Y, other.Y) }; }
    };
}

#define VECTOR2_EXPAND(v) v.X, v.Y

#endif // UTILITIES_MATH_HPP
