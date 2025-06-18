#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <exception>

template <typename T>
struct vec2 {
public:
  T x, y;
};

typedef vec2<int> vec2i;
typedef vec2<float> vec2f;
typedef vec2<double> vec2d;

#endif // UTILS_HPP
