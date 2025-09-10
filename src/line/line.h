#ifndef LINE_H
#define LINE_H

#define CURVE_SEGMENTS 16 // Number of segments to use per curve (higher = smoother, but more expensive)

constexpr float CURVE_SEGMENTS_INVERSE = 1.0f / CURVE_SEGMENTS; // Precomputed inverse for efficiency

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
  }

  //Implement copy constructor
  Line(const Line& other) {
    vertices = other.vertices;

    errorHandler = other.errorHandler;

    generateMeshData(); // Regenerate the mesh data, otherwise it won't render
  }

  void render() const;
  void setPoints(const std::vector<vec2f> &points) {
    vertices.clear();
    generateMesh(points);
    generateMeshData();
  }

private:
  unsigned int VAO{}, VBO{};
  std::vector<vec2f> vertices;

  ErrorHandler* errorHandler;

  void generateMesh(const std::vector<vec2f> &points);
  void generateMeshData();
  void addSegment(const vec2f& start, const vec2f& end);

  static vec2f catmullRom(const vec2f& p0, const vec2f& p1, const vec2f& p2, const vec2f& p3, float t);
};


#endif //LINE_H