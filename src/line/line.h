#ifndef LINE_H
#define LINE_H

#include <vector>

#include "../utils.hpp"
#include "../error_handler/error_handler.h"
#include "../window/window.hpp"

class Line {
public:
  explicit Line(ErrorHandler* errorHandler,
                const std::string& shaderPath = "res/shaders/default");
  ~Line() = default;

  void addPoint(const vec2f& point) {
    points.push_back(point);
  }

  void render(const Window& window, float scale, const vec2f& offset);

private:
  std::vector<vec2f> points;
};


#endif //LINE_H