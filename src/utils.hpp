#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cmath>

template <typename T>
struct vec2 {
    // Make sure we don't use vec2 with non-arithmetic types
    static_assert(std::is_arithmetic_v<T>, "vec2 can only be used with arithmetic types");

    T x = static_cast<T>(0), y = static_cast<T>(0);

    // Definition
    vec2() = default;
    explicit vec2(T value) : x(value), y(value) {}
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
        if (other.zero()) throw std::runtime_error("Division by zero in vec2 division");
        return vec2(x / other.x, y / other.y);
    }
    [[nodiscard]] vec2 operator/(const T& scalar) const {
        if (scalar == 0) throw std::runtime_error("Division by zero in vec2 division");
        return vec2(x / scalar, y / scalar);
    }
    [[nodiscard]] vec2 operator%(const vec2& other) const {
        if (other.zero()) throw std::runtime_error("Modulo by zero in vec2 modulo");
        return vec2(std::fmod(x, other.x), std::fmod(y, other.y));
    }
    [[nodiscard]] vec2 operator%(const T& scalar) const {
        if (scalar == 0) throw std::runtime_error("Modulo by zero in vec2 modulo");
        return vec2(std::fmod(x, scalar), std::fmod(y, scalar));
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
        if (other.zero()) throw std::runtime_error("Division by zero in vec2 division");
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
    vec2& operator%=(const vec2& other) {
        if (other.zero()) throw std::runtime_error("Modulo by zero in vec2 modulo");
        x = std::fmod(x, other.x);
        y = std::fmod(y, other.y);
        return *this;
    }
    vec2& operator%=(const T& scalar) {
        if (scalar == 0) throw std::runtime_error("Modulo by zero in vec2 modulo");
        x = std::fmod(x, scalar);
        y = std::fmod(y, scalar);
        return *this;
    }

    // Unary negation
    vec2 operator-() const { return vec2(-x, -y); }

    // Math utility functions
    [[nodiscard]] vec2 floor() const { return vec2(std::floor(x), std::floor(y)); }
    [[nodiscard]] vec2 ceil() const { return vec2(std::ceil(x), std::ceil(y)); }
    [[nodiscard]] vec2 round() const { return vec2(std::round(x), std::round(y)); }
    [[nodiscard]] vec2 abs() const { return vec2(std::abs(x), std::abs(y)); }
    [[nodiscard]] vec2 clamp(const vec2& min, const vec2& max) const {
        return vec2(std::clamp(x, min.x, max.x), std::clamp(y, min.y, max.y));
    }

    // Special vector functions
    [[nodiscard]] T length() const {
        const T absx = static_cast<T>(std::abs(x)), absy = static_cast<T>(std::abs(y));

        if (absx == 0) return absy;
        if (absy == 0) return absx;
        if (absx == absy) return static_cast<T>(std::sqrt(2)) * absx;
        return static_cast<T>(std::sqrt(x * x + y * y));
    }
    [[nodiscard]] T dot(const vec2& other) const { return x * other.x + y * other.y; }
    [[nodiscard]] vec2 normalized() const {
        T len = length();
        if (len == 0) throw std::runtime_error("Normalization of zero-length vector");
        return vec2(x / len, y / len);
    }
    [[nodiscard]] vec2 perpendicular() const { return vec2(-y, x); } // 90 degree rotation (negateY + invert)

    // Other useful functions
    [[nodiscard]] vec2 negateX() { return vec2(-x, y); }
    [[nodiscard]] vec2 negateY() { return vec2(x, -y); }
    [[nodiscard]] vec2 invert() { return vec2(y, x); }
    [[nodiscard]] bool zero() const { return std::fpclassify(x) == FP_ZERO && std::fpclassify(y) == FP_ZERO; }
  
    // Output
    friend std::ostream& operator<<(std::ostream& os, const vec2& v) {
        return os << "vec2(" << v.x << ", " << v.y << ")";
    }

    // Casting
    template <typename U>
    explicit operator vec2<U>() const {
        // Make sure we don't use vec2 with non-arithmetic types
        static_assert(std::is_arithmetic_v<U>, "vec2 can only be used with arithmetic types");
        return vec2<U>(static_cast<U>(x), static_cast<U>(y));
    }
};

// 2D vector types
typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

#endif // UTILS_HPP
