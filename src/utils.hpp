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

void printException(const std::exception& e) {
  std::vector<std::string> messages;
  const std::exception* current = &e;

  while (current) {
    messages.push_back(current->what());
    try {
      std::rethrow_if_nested(*current);
    } catch (const std::exception& nested) {
      current = &nested;
    } catch (...) {
      messages.push_back("ERROR::xxxxxx::UNKNOWN ERROR");
      break;
    }
  }

  int level = 0;
  for (const std::string& message : messages) {
    std::cerr << std::string(level, ' ') << "ERROR::" << message << std::endl;
    level++;
  }
}

#endif // UTILS_HPP
