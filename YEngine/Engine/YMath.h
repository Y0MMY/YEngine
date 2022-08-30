#pragma once
#include <math.h>

constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template <typename T>
constexpr auto sq(const T& x) noexcept
{
	return x * x;
}

template<typename T>
T wrap_angle(T theta) noexcept
{
	constexpr T twoPi = (T)2 * (T)PI_D;
	const T mod = (T)fmod(theta, twoPi);
	if (mod > (T)PI_D)
	{
		return mod - twoPi;
	}
	else if (mod < -(T)PI_D)
	{
		return mod + twoPi;
	}
	return mod;
}

template<typename T>
constexpr T interpolate(const T& src, const T& dst, float alpha) noexcept
{
	return src + (dst - src) * alpha;
}

template<typename T>
constexpr T to_rad(T deg) noexcept
{
	return deg * PI / (T)180.0;
}

template<typename T>
constexpr T gauss(T x, T sigma) noexcept
{
	const auto ss = sq(sigma);
	return ((T)1.0 / sqrt((T)2.0 * (T)PI_D * ss)) * exp(-sq(x) / ((T)2.0 * ss));
}

template <class T>
constexpr T Abs(T value) { return value >= static_cast<T>(0) ? value : -value; }

template <class T>
constexpr T Max(T a, T b) { return a > b ? a : b; }

template <class T>
constexpr T Max3(T a, T b, T c) { return Max(a, Max(b, c)); }

template <class T>
constexpr T Min(T a, T b) { return a < b ? a : b; }

template <typename T>
constexpr T Clamp(T x, T a, T b) { return x < a ? a : (x > b ? b : x); }

template <class T>
constexpr T Ceil(T x) { return ceil(x); }

class Vector2
{
public:
    Vector2()
    {
        x = 0;
        y = 0;
    }

    Vector2(const Vector2& vector)
    {
        this->x = vector.x;
        this->y = vector.y;
    }

    Vector2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }

    Vector2(int x, int y)
    {
        this->x = static_cast<float>(x);
        this->y = static_cast<float>(y);
    }

    Vector2(uint32_t x, uint32_t y)
    {
        this->x = static_cast<float>(x);
        this->y = static_cast<float>(y);
    }

    Vector2(float x)
    {
        this->x = x;
        this->y = x;
    }

    ~Vector2() = default;

    //= ADDITION ===============================
    Vector2 operator+(const Vector2& b) const
    {
        return Vector2
        (
            this->x + b.x,
            this->y + b.y
        );
    }

    void operator+=(const Vector2& b)
    {
        this->x += b.x;
        this->y += b.y;
    }
    //==========================================

    //= MULTIPLICATION =======================================================================================
    Vector2 operator*(const Vector2& b) const
    {
        return Vector2(x * b.x, y * b.y);
    }

    void operator*=(const Vector2& b)
    {
        x *= b.x;
        y *= b.y;
    }

    Vector2 operator*(const float value) const
    {
        return Vector2(x * value, y * value);
    }

    void operator*=(const float value)
    {
        x *= value;
        y *= value;
    }
    //=======================================================================================================


    //= SUBTRACTION ===============================================================
    Vector2 operator-(const Vector2& b) const { return Vector2(x - b.x, y - b.y); }
    Vector2 operator-(const float value) const { return Vector2(x - value, y - value); }

    void operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
    }
    //=============================================================================

    //= DIVISION ========================================================================
    Vector2 operator/(const Vector2& rhs) const { return Vector2(x / rhs.x, y / rhs.y); }
    Vector2 operator/(const float rhs) const { return Vector2(x / rhs, y / rhs); }

    void operator/=(const Vector2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
    }
    //===================================================================================

    // Returns the length
  /*  [[nodiscard]] float Length() const { return Helper::Sqrt(x * x + y * y); }*/
    // Returns the squared length
    [[nodiscard]] float LengthSquared() const { return x * x + y * y; }

    // Returns the distance between to vectors
    //static inline float Distance(const Vector2& a, const Vector2& b) { return (b - a).Length(); }
    // Returns the squared distance between to vectors
    static inline float DistanceSquared(const Vector2& a, const Vector2& b) { return (b - a).LengthSquared(); }

    bool operator==(const Vector2& b) const
    {
        return x == b.x && y == b.y;
    }

    bool operator!=(const Vector2& b) const
    {
        return x != b.x || y != b.y;
    }

    [[nodiscard]] const float* Data() const { return &x; }
    std::string ToString() const;

    float x;
    float y;
};

class Vector5
{
public:
    Vector5()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
        h = 0;
    }

    Vector5(float x, float y, float z, float w, float h)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
        this->h = 0;
    }

    Vector5(float value)
    {
        this->x = value;
        this->y = value;
        this->z = value;
        this->w = value;
        this->h = 0;
    }

   /* Vector5(const Vector3& value, float w);
    Vector5(const Vector3& value);*/

    ~Vector5() = default;

    bool operator ==(const Vector5& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool operator !=(const Vector5& rhs) const
    {
        return !(*this == rhs);
    }

    Vector5 operator*(const float value) const
    {
        return Vector5(
            x * value,
            y * value,
            z * value,
            w * value,
            h * value
        );
    }

    void operator*=(const float value)
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        h*= value;
    }

   

    float x, y, z, w, h;


};