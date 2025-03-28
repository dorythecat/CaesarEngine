#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <vector>
#include <exception>

typedef struct {
  int x, y;
} vec2i;

typedef struct {
  float x, y;
} vec2f;

typedef struct {
  double x, y;
} vec2d;

void printException(const std::exception& e);

#endif // UTILS_HPP
