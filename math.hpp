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

        inline vector_type operator+(vector_type const& other) const { return vector_type { X + other.X, Y + other.Y }; };
        inline vector_type operator-(vector_type const& other) const { return vector_type { X - other.X, Y - other.Y }; };
        inline vector_type operator-() const { return vector_type { -X, -Y }; }
        inline vector_type operator*(TNumeric scalar) const { return vector_type { X * scalar, Y * scalar }; };
        inline vector_type operator/(TNumeric scalar) const { return vector_type { X / scalar, Y / scalar }; };
        inline bool operator==(vector_type const& other) const { return X == other.X && Y == other.Y; };
        inline bool operator!=(vector_type const& other) const { return X == other.X && Y == other.Y; };

        inline vector_type ceil() const { return vector_type { std::ceil(X), std::ceil(Y) }; };
        inline vector_type round() const { return vector_type { std::round(X), std::round(Y) }; };
        inline vector_type floor() const { return vector_type { std::floor(X), std::floor(Y) }; };

        inline TNumeric length_squared() const { return X * X + Y * Y; }
        inline TNumeric length() const { return sqrt(length_squared()); }

        static vector_type max(vector_type const& a, vector_type const& b) { return vector_type { std::max(a.X, b.X), std::max(a.Y, b.Y) }; }
        static vector_type min(vector_type const& a, vector_type const& b) { return vector_type { std::min(a.X, b.X), std::min(a.Y, b.Y) }; }
        static vector_type abs(vector_type const& a, vector_type const& b) { return vector_type { std::abs(a.X, b.X), std::abs(a.Y, b.Y) }; }

        template<typename TValueTypeTo>
        inline static Vector2<TValueTypeTo> static_cast_to(vector_type const& v) { return Vector2<TValueTypeTo> { static_cast<TValueTypeTo>(v.X), static_cast<TValueTypeTo>(v.Y) }; }
        template<typename TValueTypeFrom>
        inline static vector_type static_cast_from(Vector2<TValueTypeFrom> const& v) { return vector_type { static_cast<value_type>(v.X), static_cast<value_type>(v.Y) }; }
    };

    /*
     * @brief SAT implementation in 1-dimensional space for lines.
     * @brief it also can be used as basic check for 1+ dimensional spaces.
     */
    template<typename TNumeric>
    inline bool collides(
            TNumeric begin0, TNumeric end0
          , TNumeric begin1, TNumeric end1
          , TNumeric& beginOut, TNumeric& endOut
    ) {
        // some rules:
        // 1. line 0 must be 'higher' than line 1
        // 2. begin 'point' must be lower than end 'point'

        if(begin0 > end0)
            std::swap(begin0, end0);
        if(begin1 > end1)
            std::swap(begin1, end1);

        if(end0 < end1)
        {
            std::swap(begin0, begin1);
            std::swap(end0, end1);
        }

        // in generally we have 4 separating 'axises' (it consist of begin and end of both lines)
        // but we gurantee that line0 end always higher
        // so it means that we can check only points of line1 because it always will be included (or be part of) in line0 at intersection
        // and moreover we can check only end of line1 to be inside line0 (because it can be inside ot outside lower-only).

        auto collides = /*end1 <= end0 && */end1 >= begin0;
        if(collides)
        {
            endOut = end1;
            beginOut = std::min(begin0, begin1);
        }
        return collides;
    }
}

#define VECTOR2_EXPAND(v) (v).X, (v).Y

#endif // UTILITIES_MATH_HPP
