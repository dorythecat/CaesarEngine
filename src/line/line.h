#ifndef LINE_H
#define LINE_H

#include <glad/glad.h>

#include <vector>

#include "../utils.hpp"
#include "../error_handler/error_handler.h"
#include "../window/window.hpp"
#include "../shader/shader.hpp"

class Line {
public:
  Line(ErrorHandler* errorHandler,
       std::vector<vec2f> points,
       const std::string& shaderPath = "res/shaders/line");
  ~Line() = default;

  void render(const Window& window, float scale, const vec2f& offset) const;

private:
  unsigned int VAO{}, VBO{}, EBO{};
  std::vector<vec2f> vertices;
  std::vector<unsigned int> indices;
  Shader shader;

  ErrorHandler* errorHandler;

  void generateMesh(std::vector<vec2f> points);
  void generateMeshData();
};


#endif //LINE_H