#ifndef LINE_H
#define LINE_H

#include <glad/glad.h>

#include <vector>

#include "../utils.hpp"
#include "../error_handler/error_handler.h"

class Line {
public:
  explicit Line(ErrorHandler* errorHandler) : errorHandler(errorHandler) {}
  Line(ErrorHandler* errorHandler,
       const std::vector<vec2f> &points);
  ~Line() noexcept {
    // Clean up the mesh data
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  void render() const;

private:
  unsigned int VAO{}, VBO{}, EBO{};
  std::vector<vec2f> vertices;
  std::vector<unsigned int> indices;

  ErrorHandler* errorHandler;

  void generateMesh(const std::vector<vec2f> &points);
  void generateMeshData();
  void addSegment(const vec2f& start, const vec2f& end);
};


#endif //LINE_H