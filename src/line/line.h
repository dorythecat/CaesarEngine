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

  static vec2f catmullRom(const vec2f& p0, const vec2f& p1, const vec2f& p2, const vec2f& p3, float t) {
    // See https://en.wikipedia.org/wiki/Centripetal_Catmull%E2%80%93Rom_spline

    // Knot parameters
    constexpr float t0 = 0.0f;
    const float t1 = t0 + std::pow((p1 - p0).length(), 0.5f);
    const float t2 = t1 + std::pow((p2 - p1).length(), 0.5f);
    const float t3 = t2 + std::pow((p3 - p2).length(), 0.5f);

    // Rescale t to the range [t1, t2], so it goes from P1 to P2
    t = t1 + t * (t2 - t1);

    // Calculate the basis functions
    const vec2f A1 = p0 * (t1 - t) / (t1 - t0) + p1 * (t - t0) / (t1 - t0);
    const vec2f A2 = p1 * (t2 - t) / (t2 - t1) + p2 * (t - t1) / (t2 - t1);
    const vec2f A3 = p2 * (t3 - t) / (t3 - t2) + p3 * (t - t2) / (t3 - t2);

    const vec2f B1 = A1 * (t2 - t) / (t2 - t0) + A2 * (t - t0) / (t2 - t0);
    const vec2f B2 = A2 * (t3 - t) / (t3 - t1) + A3 * (t - t1) / (t3 - t1);

    return B1 * (t2 - t) / (t2 - t1) + B2 * (t - t1) / (t2 - t1); // Final point
  }
};


#endif //LINE_H