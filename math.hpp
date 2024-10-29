#ifndef UTILITIES_MATH_HPP
#define UTILITIES_MATH_HPP

#include <cmath>
#include <ostream>

#define fraction_inlined(x) (x - std::floor(x))

namespace Utilities::Math
{
    template<typename TNumeric>
    inline TNumeric fraction(TNumeric const& x) noexcept {
        return fraction_inlined(x);
    }

    template<typename TNumeric>
    struct Vector2
    {
        using value_type = TNumeric;
        using vector_type = Vector2<value_type>;

        inline static vector_type Zero() noexcept { return vector_type { 0, 0 }; };
        inline static vector_type Identity() noexcept { return vector_type { 1, 1 }; };
        inline static vector_type XAxis() noexcept { return vector_type { 1, 0 }; };
        inline static vector_type YAxis() noexcept { return vector_type { 0, 1 }; };

        value_type X, Y;

        inline vector_type operator+(vector_type const& other) const noexcept { return vector_type { X + other.X, Y + other.Y }; };
        inline vector_type operator-(vector_type const& other) const noexcept { return vector_type { X - other.X, Y - other.Y }; };
        inline vector_type operator-() const noexcept { return vector_type { -X, -Y }; }
        inline vector_type operator*(TNumeric scalar) const noexcept { return vector_type { X * scalar, Y * scalar }; };
        inline vector_type operator/(TNumeric scalar) const noexcept { return vector_type { X / scalar, Y / scalar }; };
        inline bool operator==(vector_type const& other) const noexcept { return X == other.X && Y == other.Y; };
        inline bool operator!=(vector_type const& other) const noexcept { return X == other.X && Y == other.Y; };
        inline TNumeric operator[](size_t index) const
        {
            switch(index) {
                case(0): return X;
                case(1): return Y;
            }
            throw std::out_of_range("");
        }

        inline vector_type ceil() const noexcept { return vector_type { std::ceil(X), std::ceil(Y) }; };
        inline vector_type round() const noexcept { return vector_type { std::round(X), std::round(Y) }; };
        inline vector_type floor() const noexcept { return vector_type { std::floor(X), std::floor(Y) }; };

        inline TNumeric length_squared() const noexcept { return X * X + Y * Y; }
        inline TNumeric length() const noexcept { return sqrt(length_squared()); }
        inline vector_type normalize() const noexcept
        {
            if(!X && !Y) return *this;
            TNumeric const length = length();
            return vector_type { X / length, Y / length };
        }
        
        inline static vector_type max(vector_type const& a, vector_type const& b) noexcept { return vector_type { std::max(a.X, b.X), std::max(a.Y, b.Y) }; }
        inline static vector_type min(vector_type const& a, vector_type const& b) noexcept { return vector_type { std::min(a.X, b.X), std::min(a.Y, b.Y) }; }
        inline static vector_type abs(vector_type const& a, vector_type const& b) noexcept { return vector_type { std::abs(a.X, b.X), std::abs(a.Y, b.Y) }; }
        inline static vector_type compound(vector_type const& a, vector_type const& b) noexcept { return vector_type { a.X * b.X, a.Y * b.Y }; }

        template<typename TValueTypeTo>
        inline static Vector2<TValueTypeTo> static_cast_to(vector_type const& v) noexcept { return Vector2<TValueTypeTo> { static_cast<TValueTypeTo>(v.X), static_cast<TValueTypeTo>(v.Y) }; }
        template<typename TValueTypeFrom>
        inline static vector_type static_cast_from(Vector2<TValueTypeFrom> const& v) noexcept { return vector_type { static_cast<value_type>(v.X), static_cast<value_type>(v.Y) }; }

        inline friend std::ostream& operator<<(std::ostream& stream, vector_type const& v) noexcept { return stream << "{ " << v.X << ", " << v.Y << " }"; }
    };

    template<typename TNumeric>
    struct Matrix2
    {
        using value_type = TNumeric;
        using vector_type = Vector2<value_type>;

        vector_type R0 {1,0}, R1 {0,1};

        constexpr Matrix2() = default;
        constexpr Matrix2(value_type xr0, value_type yr0, value_type xr1, value_type yr1) : R0(vector_type{xr0, yr0}), R1(vector_type{xr1, yr1}) {}
        constexpr Matrix2(vector_type r0, vector_type r1) : R0(r0), R1(r1) {}
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
    ) noexcept {
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
