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
    vec2 operator==(const vec2& other) const { return x == other.x && y == other.y; }
    vec2 operator!=(const vec2& other) const { return x != other.x || y != other.y; }

    // Basic arithmetic
    vec2 operator+(const vec2& other) const { return vec2(x + other.x, y + other.y); }
    vec2 operator+(const T& scalar) const { return vec2(x + scalar, y + scalar); }
    vec2 operator-(const vec2& other) const { return vec2(x - other.x, y - other.y); }
    vec2 operator-(const T& scalar) const { return vec2(x - scalar, y - scalar); }
    vec2 operator*(const vec2& other) const { return vec2(x * other.x, y * other.y); }
    vec2 operator*(const T& scalar) const { return vec2(x * scalar, y * scalar); }
    vec2 operator/(const vec2& other) const {
        if (other.x == 0 || other.y == 0) throw std::runtime_error("Division by zero in vec2 division");
        return vec2(x / other.x, y / other.y);
    }
    vec2 operator/(const T& scalar) const {
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
  
    // Output
    friend std::ostream& operator<<(std::ostream& os, const vec2& v) {
        os << "vec2(" << v.x << ", " << v.y << ")";
        return os;
    }
};

// 2D vector types
typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

#endif // UTILS_HPP
