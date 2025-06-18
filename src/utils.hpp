#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <cmath>

template <typename T>
struct vec2 {
public:
  T x, y;

  // Definition
  vec2() : x(0), y(0) {}
  vec2(T x, T y) : x(x), y(y) {}
  vec2(const vec2<T>& other) : x(other.x), y(other.y) {}
  vec2<T>& operator=(const vec2<T>& other) {
    if (this != &other) {
      x = other.x;
      y = other.y;
    }
    return *this;
  }
  
  // Comparison
  vec2<T> operator==(const vec2<T>& other) const {
    return vec2<T>(x == other.x, y == other.y);
  }
  vec2<T> operator!=(const vec2<T>& other) const {
    return vec2<T>(x != other.x, y != other.y);
  }

  // Arithmetic
  vec2<T> operator+(const vec2<T>& other) const {
    return vec2<T>(x + other.x, y + other.y);
  }
  vec2<T> operator+(const T& scalar) const {
    return vec2<T>(x + scalar, y + scalar);
  }
  vec2<T> operator-(const vec2<T>& other) const {
    return vec2<T>(x - other.x, y - other.y);
  }
  vec2<T> operator-(const T& scalar) const {
    return vec2<T>(x - scalar, y - scalar);
  }
  vec2<T> operator*(const vec2<T>& other) const {
    return vec2<T>(x * other.x, y * other.y);
  }
  vec2<T> operator*(const T& scalar) const {
    return vec2<T>(x * scalar, y * scalar);
  }
  vec2<T> operator/(const vec2<T>& other) const {
    if (other.x == 0 || other.y == 0) {
      throw std::runtime_error("Division by zero in vec2 division");
    }
    return vec2<T>(x / other.x, y / other.y);
  }
  vec2<T> operator/(const T& scalar) const {
    if (scalar == 0) {
      throw std::runtime_error("Division by zero in vec2 division");
    }
    return vec2<T>(x / scalar, y / scalar);
  }

  // In-place Arithmetic
  vec2<T>& operator+=(const vec2<T>& other) {
    x += other.x;
    y += other.y;
    return *this;
  }
  vec2<T>& operator+=(const T& scalar) {
    x += scalar;
    y += scalar;
    return *this;
  }
  vec2<T>& operator-=(const vec2<T>& other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }
  vec2<T>& operator-=(const T& scalar) {
    x -= scalar;
    y -= scalar;
    return *this;
  }
  vec2<T>& operator*=(const vec2<T>& other) {
    x *= other.x;
    y *= other.y;
    return *this;
  }
  vec2<T>& operator*=(const T& scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }
  vec2<T>& operator/=(const vec2<T>& other) {
    if (other.x == 0 || other.y == 0) {
      throw std::runtime_error("Division by zero in vec2 division");
    }
    x /= other.x;
    y /= other.y;
    return *this;
  }
  vec2<T>& operator/=(const T& scalar) {
    if (scalar == 0) {
      throw std::runtime_error("Division by zero in vec2 division");
    }
    x /= scalar;
    y /= scalar;
    return *this;
  }

  // Special Functions
  T length() const {
    if (x == 0 && y == 0) return 0;
    if (x == 0) return std::abs(y);
    if (y == 0) return std::abs(x);
    return std::sqrt(x * x + y * y);
  }
  T dot(const vec2<T>& other) const {
    return x * other.x + y * other.y;
  }
  vec2<T> normalized() const {
    T len = length();
    if (len == 0) {
      throw std::runtime_error("Normalization of zero-length vector");
    }
    return vec2<T>(x / len, y / len);
  }
  
  // Output
  friend std::ostream& operator<<(std::ostream& os, const vec2<T>& v) {
    os << "vec2(" << v.x << ", " << v.y << ")";
    return os;
  }
};

typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

#endif // UTILS_HPP
