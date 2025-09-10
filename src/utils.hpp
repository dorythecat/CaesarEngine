#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cmath>

template <typename T>
struct vec2 {
    // Make sure we don't use vec2 with non-arithmetic types
    static_assert(std::is_arithmetic_v<T>, "vec2 can only be used with arithmetic types");

    T x, y;

    // Definition
    vec2() : x(0), y(0) {}
    vec2(T x, T y) : x(x), y(y) {}
    vec2(const vec2& other) : x(other.x), y(other.y) {}
    vec2& operator=(const vec2& other) {
        if (this != &other) {
            x = other.x;
            y = other.y;
        } return *this;
    }
  
    // Comparison
    [[nodiscard]] bool operator==(const vec2& other) const { return x == other.x && y == other.y; }
    [[nodiscard]] bool operator!=(const vec2& other) const { return x != other.x || y != other.y; }
    [[nodiscard]] bool operator<(const vec2& other) const { return (x < other.x) || (x == other.x && y < other.y); }
    [[nodiscard]] bool operator<=(const vec2& other) const { return (x < other.x) || (x == other.x && y <= other.y); }
    [[nodiscard]] bool operator>(const vec2& other) const { return (x > other.x) || (x == other.x && y > other.y); }
    [[nodiscard]] bool operator>=(const vec2& other) const { return (x > other.x) || (x == other.x && y >= other.y); }

    // Basic arithmetic
    [[nodiscard]] vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y); }
    [[nodiscard]] vec2 operator+(const T& scalar) const { return vec2(x + scalar, y + scalar); }
    [[nodiscard]] vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y); }
    [[nodiscard]] vec2 operator-(const T& scalar) const { return vec2(x - scalar, y - scalar); }
    [[nodiscard]] vec2 operator*(const vec2& other) const { return vec2(x * other.x, y * other.y); }
    [[nodiscard]] vec2 operator*(const T& scalar) const { return vec2(x * scalar, y * scalar); }
    [[nodiscard]] vec2 operator/(const vec2& other) const {
        if (other.x == 0 || other.y == 0) throw std::runtime_error("Division by zero in vec2 division");
        return vec2(x / other.x, y / other.y);
    }
    [[nodiscard]] vec2 operator/(const T& scalar) const {
        if (scalar == 0) throw std::runtime_error("Division by zero in vec2 division");
        return vec2(x / scalar, y / scalar);
    }

    // In-place basic arithmetic
    vec2& operator+=(const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    vec2& operator+=(const T& scalar) {
        x += scalar;
        y += scalar;
        return *this;
    }
    vec2& operator-=(const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    vec2& operator-=(const T& scalar) {
        x -= scalar;
        y -= scalar;
        return *this;
    }
    vec2& operator*=(const vec2& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    vec2& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    vec2& operator/=(const vec2& other) {
        if (other.x == 0 || other.y == 0) throw std::runtime_error("Division by zero in vec2 division");
        x /= other.x;
        y /= other.y;
        return *this;
    }
    vec2& operator/=(const T& scalar) {
        if (scalar == 0) throw std::runtime_error("Division by zero in vec2 division");
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Unary negation
    vec2 operator-() const { return vec2(-x, -y); }

    // Special vector functions
    [[nodiscard]] T length() const {
        if (x == 0 && y == 0) return 0;
        if (x == 0) return std::abs(y);
        if (y == 0) return std::abs(x);
        return std::sqrt(x * x + y * y);
    }
    [[nodiscard]] T dot(const vec2& other) const { return x * other.x + y * other.y; }
    [[nodiscard]] vec2 normalized() const {
        T len = length();
        if (len == 0) throw std::runtime_error("Normalization of zero-length vector");
        return vec2(x / len, y / len);
    }
    [[nodiscard]] vec2 perpendicular() const { return vec2(-y, x); } // 90 degree rotation
  
    // Output
    friend std::ostream& operator<<(std::ostream& os, const vec2& v) {
        os << "vec2(" << v.x << ", " << v.y << ")";
        return os;
    }

    // Casting
    template <typename U>
    explicit operator vec2<U>() const { return vec2<U>(static_cast<U>(x), static_cast<U>(y)); }
};

// 2D vector types
typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

#endif // UTILS_HPP
