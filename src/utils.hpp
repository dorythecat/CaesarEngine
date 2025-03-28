#ifndef UTILS_HPP
#define UTILS_HPP

typedef struct {
  int x, y;
} vec2i;

typedef struct {
  float x, y;
} vec2f;

typedef struct {
  double x, y;
} vec2d;

void printException(const std::exception& e, int level = 0) {
  std::cerr << std::string(level, ' ') << "ERROR: " << e.what() << std::endl;
  try {
    std::rethrow_if_nested(e);
  } catch (const std::exception& e) {
    printException(e, level + 1);
  } catch (...) {}
}

#endif // UTILS_HPP
