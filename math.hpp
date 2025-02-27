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
        inline static value_type dot(vector_type const& a, vector_type const& b) noexcept { return (a.X * b.X) + (a.Y * b.Y); }
        inline static double cos(vector_type const& a, vector_type const& b) noexcept { return dot(a, b) / (a.length() * b.length()); }
        inline static double angle(vector_type const& a, vector_type const& b) noexcept { return std::acos(cos(a, b)); }
        inline static double project(vector_type const& a, vector_type const& b) noexcept { return dot(a, b) / b.length(); }

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
    template<typename TNumeric, size_t size_>
    struct Vector
    {
        constexpr static size_t Size = size_;
        constexpr size_t size() const noexcept { return Size; }

        using value_type = TNumeric;
        using vector_type = Vector<TNumeric, Size>;
        using array_type = value_type[Size];

        array_type Value[Size];

        constexpr value_type& operator[](size_t index) noexcept { return Value[index]; }
        constexpr value_type const& operator[](size_t index) const noexcept { return Value[index]; }

        constexpr vector_type operator+(vector_type const& other) const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = *this[i] + other[i];
            return v;
        };
        constexpr vector_type operator-(vector_type const& other) const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = *this[i] - other[i];
            return v;
        };
        constexpr vector_type operator-() const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = -*this[i];
            return v;
        };
        constexpr vector_type operator*(value_type scalar) const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = *this[i] * scalar;
            return v;
        };
        constexpr vector_type operator/(value_type scalar) const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = *this[i] / scalar;
            return v;
        };
        constexpr bool operator==(vector_type const& other) const noexcept {
            for(size_t i = 0; i < size(); ++i)
                if(*this[i] != other[i])
                    return false;
            return true;
        };
        constexpr bool operator!=(vector_type const& other) const noexcept  {
            for(size_t i = 0; i < size(); ++i)
                if(Value[i] != other.Value[i])
                    return true;
            return false;
        };

        constexpr value_type length_squared() const noexcept {
            value_type sum = 0;
            for(size_t i = 0; i < size(); ++i)
                sum += *this[i] * *this[i];
            return sum;
        };
        constexpr TNumeric length() const noexcept { return sqrt(length_squared()); }
        constexpr vector_type normalize() const noexcept
        {
            if(!*this[0] && !*this[1] && !*this[2]) return *this;
            return *this / length();
        }

        constexpr vector_type ceil() const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = std::ceil(*this[i]);
            return v;
        };
        constexpr vector_type round() const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = std::round(*this[i]);
            return v;
        };
        constexpr vector_type floor() const noexcept {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = std::floor(*this[i]);
            return v;
        };
        constexpr vector_type abs() {
            vector_type v;
            for(size_t i = 0; i < size(); ++i)
                v[i] = std::abs(*this[i]);
            return v;
        };

        constexpr static vector_type max(vector_type const& a, vector_type const& b) noexcept {
            vector_type v;
            for(size_t i = 0; i < a.size(); ++i)
                v[i] = std::max(a[i], b[i]);
            return v;
        };
        constexpr static vector_type min(vector_type const& a, vector_type const& b) noexcept {
            vector_type v;
            for(size_t i = 0; i < a.size(); ++i)
                v[i] = std::min(a[i], b[i]);
            return v;
        };
        constexpr static vector_type compound(vector_type const& a, vector_type const& b) noexcept {
            vector_type v;
            for(size_t i = 0; i < a.size(); ++i)
                v[i] = a[i] * b[i];
            return v;
        };

        constexpr void fill(value_type&& v) noexcept {
            for(size_t i = 0; i < size(); ++i)
                *this[i] = v;
        }

        constexpr Vector() = default;
        constexpr Vector(value_type&& v) : Value({v, v, v}) {}
        constexpr Vector(value_type v) : Value({v, v, v}) {}
        constexpr Vector(array_type&& arr) : Value(arr) {}
        constexpr Vector(array_type const& arr) : Value(arr) {}

        constexpr static vector_type Zero() noexcept {
            vector_type v;
            for(size_t i = 0; i < v.size(); ++i)
                v[i] = 0;
            return v;
        }
        constexpr static vector_type Axis(size_t index) noexcept {
            vector_type v;
            for(size_t i = 0; i < v.size(); ++i)
                v[i] = 0;
            v[index] = 1;
            return v;
        }

        inline friend std::ostream& operator<<(std::ostream& stream, vector_type const& v) noexcept {
            stream << "{ ";
            for(size_t i = 0; i < v.size(); ++i)
                    stream << (i == 0 ? "" : ", ") << v[i];
            return stream << " }";
        }
    };

    template<typename TNumeric, size_t rows_, size_t columns_>
    struct Matrix
    {
        constexpr static size_t Rows = rows_;
        constexpr static size_t Columns = columns_;
        constexpr static size_t Size = Rows * Columns;
        constexpr size_t rows() const noexcept { return Rows; }
        constexpr size_t columns() const noexcept { return Columns; }
        constexpr size_t size() const noexcept { return Size; }

        using value_type = TNumeric;
        using row_t = Vector<value_type, Columns>;
        using column_t = Vector<value_type, Rows>;
        using array_type = row_t[Rows];
        using matrix_type = Matrix<value_type, Rows, Columns>;
        using matrix_transposed_type = Matrix<value_type, Rows, Columns>;

        array_type Value;

        constexpr row_t& operator[](size_t index) noexcept { return Value[index]; }
        constexpr row_t const& operator[](size_t index) const noexcept { return Value[index]; }

        constexpr matrix_type operator+(matrix_type const& other) const noexcept {
            matrix_type m;
            for(size_t i = 0; i < rows(); ++i)
                m[i] = *this[i] + other[i];
            return m;
        };
        constexpr matrix_type operator-(matrix_type const& other) const noexcept {
            matrix_type m;
            for(size_t i = 0; i < rows(); ++i)
                m[i] = *this[i] - other[i];
            return m;
        };
        constexpr matrix_type operator-() const noexcept {
            matrix_type m;
            for(size_t i = 0; i < rows(); ++i)
                m[i] = -*this[i];
            return m;
        };
        //TODO: Implement for linked matrixes. Currently only squared matrixes supported.
        constexpr matrix_type operator*(matrix_type const& other) const {
            matrix_type m;
            for(size_t r = 0; r < rows(); ++r)
                for(size_t c = 0; r < columns(); ++c)
                {
                    value_type compound = 0;
                    for(size_t i = 0; i < std::min(rows(), columns()); ++i)
                        compound += *this[r][i] * other[c][i];
                    m[r][c] = compound;
                }
            return m;
        };
        constexpr row_t operator*(column_t const& other)
        {
            row_t v{0};
            for(size_t r = 0; r < std::min(other.size(), rows()); ++r)
            {
                value_type compound = 0;
                for(size_t c = 0; c < std::min(other.size(), rows()); ++ c)
                    compound = ;
                v[r] = compound;
            }

            return v;
        }
        constexpr matrix_type operator*(value_type scalar) const noexcept {
            matrix_type m;
            for(size_t i = 0; i < rows(); ++i)
                m[i] = *this[i] * scalar;
            return m;
        };
        constexpr matrix_type operator/(value_type scalar) const noexcept {
            matrix_type m;
            for(size_t i = 0; i < rows(); ++i)
                m[i] = *this[i] / scalar;
            return m;
        };
        constexpr bool operator==(matrix_type const& other) const noexcept {
            for(size_t i = 0; i < rows(); ++i)
                if(*this[i] != other[i])
                    return false;
            return true;
        };
        constexpr bool operator!=(matrix_type const& other) const noexcept  {
            for(size_t i = 0; i < rows(); ++i)
                if(*this[i] != other[i])
                    return true;
            return false;
        }

        constexpr matrix_transposed_type transposed() const noexcept
        {
            matrix_transposed_type t;
            for(size_t r = 0; r < rows(); ++r)
                for(size_t c = 0; c < columns(); ++c)
                    t[c][r] = *this[r][c];
            return t;
        }
        constexpr void fill(value_type&& v) noexcept {
            for(size_t i = 0; i < rows(); ++i)
                *this[i].fill(v);
        }

        constexpr Matrix() = default;
        constexpr Matrix(value_type&& v) { fill(v); }
        constexpr Matrix(value_type v) { fill(v); }
        constexpr Matrix(array_type&& arr) : Value(arr) {}
        constexpr Matrix(array_type const& arr) : Value(arr) {}

        constexpr static matrix_type Zero() { return matrix_type(0); }
        constexpr static matrix_type Identity()
        {
            matrix_type m {0};
            for(size_t i = 0; i < std::min(Rows, Columns); ++i)
                m[i][i] = 1;
            return m;
        }

        inline friend std::ostream& operator<<(std::ostream& stream, matrix_type const& m) noexcept {
            stream << "{ ";
            for(size_t i = 0; i < m.rows(); ++i)
                    stream << (i == 0 ? "" : ", ") << m[i];
            return stream << " }";
        }
    };

    template<typename TNumeric>
    constexpr Matrix<TNumeric, 3, 3> dim2_scaling(TNumeric&& x, TNumeric&& y) noexcept
    {
        return Matrix<TNumeric, 3, 3>{{
              { x, 0, 0 }
            , { 0, y, 0 }
            , { 0, 0, 1 }
        }};
    }
    template<typename TNumeric>
    constexpr Matrix<TNumeric, 3, 3> dim2_scaling(Vector<TNumeric, 2>&& v) noexcept
    {
        return Matrix<TNumeric, 3, 3>{{
              { v[0], 0, 0 }
            , { 0, v[1], 0 }
            , { 0, 0, 1 }
        }};
    }
    template<typename TNumeric>
    constexpr Matrix<TNumeric, 3, 3> dim2_rotation(double radians) noexcept
    {
        auto const cos = std::cos(radians);
        auto const sin = std::sin(radians);
        return Matrix<TNumeric, 3, 3>{{
              { cos, sin, 0 }
            , { -sin, cos, 0 }
            , { 0, 0, 1 }
        }};
    }
    template<typename TNumeric>
    constexpr Matrix<TNumeric, 3, 3> dim2_translation(TNumeric&& x, TNumeric&& y) noexcept
    {
        return Matrix<TNumeric, 3, 3>{{
              { 1, 0, x }
            , { 0, 1, y }
            , { 0, 0, 1 }
        }};
    }
    template<typename TNumeric>
    constexpr Matrix<TNumeric, 3, 3> dim2_translation(Vector<TNumeric, 2>&& v) noexcept
    {
        return Matrix<TNumeric, 3, 3>{{
              { 1, 0, v[0] }
            , { 0, 1, v[1] }
            , { 0, 0, 1 }
        }};
    }
    */

    /*
     * @brief SAT implementation in 1-dimensional space for lines.
     * @brief it also can be used as basic check for 1+ dimensional spaces.
     */
    template<typename TNumeric>
    constexpr bool collides(
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

    template<typename TNumeric>
    struct Range
    {
        using value_type = TNumeric;
        using range_type = Range<value_type>;

        double A, B;
        constexpr double center() const noexcept { return (A + B) / 2; }
        constexpr double distance() const noexcept { return B - A; }
        constexpr bool contains(double x) const noexcept { return x >= A && x <= B; }
        constexpr bool collides(range_type&& o) const noexcept { value_type _0 { 0 }, _1 { 0 }; return Utilities::Math::collides(A, B, o.A, o.B, _0, _1); }
        constexpr bool collides(range_type&& o, value_type& beginOut, value_type& endOut) const noexcept { return Utilities::Math::collides(A, B, o.A, o.B, beginOut, endOut); }
        constexpr bool collides(range_type&& o, range_type& out) const noexcept { return Utilities::Math::collides(A, B, o.A, o.B, out.A, out.B); }
        constexpr bool collides(range_type const& o) const noexcept { value_type _0 { 0 }, _1 { 0 }; return Utilities::Math::collides(A, B, o.A, o.B, _0, _1); }
        constexpr bool collides(range_type const& o, value_type& beginOut, value_type& endOut) const noexcept { return Utilities::Math::collides(A, B, o.A, o.B, beginOut, endOut); }
        constexpr bool collides(range_type const& o, range_type& out) const noexcept { return Utilities::Math::collides(A, B, o.A, o.B, out.A, out.B); }

        constexpr range_type operator+(range_type&& o) const noexcept { return { A + o.A, B + o.B }; }
        constexpr range_type operator+(range_type const& o) const noexcept { return { A + o.A, B + o.B }; }
        constexpr range_type operator+(double x) const noexcept { return { A + x, B + x }; }
        constexpr bool operator==(range_type&& o) const noexcept { return A == o.A && B == o.B; }
        constexpr bool operator==(range_type const& o) const noexcept { return A == o.A && B == o.B; }
        constexpr bool operator!=(range_type&& o) const noexcept { return A != o.A || B != o.B; }        
        constexpr bool operator!=(range_type const& o) const noexcept { return A != o.A || B != o.B; }
    };

    template<typename TNumeric>
    struct Rectangle
    {
        using value_type = TNumeric;
        using rectangle_type = Rectangle<value_type>;
        using vector_type = Vector2<value_type>;
        using range_type = Range<value_type>;

        vector_type BottomLeft, RightTop;

        constexpr Rectangle() = default;
        constexpr Rectangle(vector_type bottomLeft, vector_type rightTop) : BottomLeft(bottomLeft), RightTop(rightTop) {}

        constexpr range_type x() const noexcept { return range_type{BottomLeft.X, RightTop.X}; }
        constexpr range_type y() const noexcept { return  range_type{BottomLeft.Y, RightTop.Y}; }

        constexpr vector_type size() const noexcept { return RightTop - BottomLeft; }
        constexpr bool contains(vector_type point) const noexcept { return x().contains(point.X) && y().contains(point.Y); }
        constexpr bool collides(rectangle_type&& o) const noexcept { range_type _0, _1; return x().collides(o.x(), _0) && y().collides(o.y(), _1); }
        constexpr bool collides(rectangle_type&& o, range_type& outX, range_type& outY) const noexcept { return x().collides(o.x(), outX) && y().collides(o.y(), outY); }
        constexpr bool collides(rectangle_type const& o) const noexcept { range_type _0, _1; return x().collides(o.x(), _0) && y().collides(o.y(), _1); }
        constexpr bool collides(rectangle_type const& o, range_type& outX, range_type& outY) const noexcept { return x().collides(o.x(), outX) && y().collides(o.y(), outY); }
    };
}

#define VECTOR2_EXPAND(v) (v).X, (v).Y

#endif // UTILITIES_MATH_HPP
