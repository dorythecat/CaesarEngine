#include "line.h"

Line::Line(ErrorHandler* errorHandler,
           const std::vector<vec2f> &points) : errorHandler(errorHandler) {
  generateMesh(points);
  generateMeshData();
}

void Line::render() const {
  if (vertices.empty()) return; // Nothing to render
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLint>(vertices.size()));
  glBindVertexArray(0);
}

void Line::generateMesh(const std::vector<vec2f> &points) {
  if (points.size() < 2) {
    errorHandler->logError("Line must have at least 2 points");
    return;
  }

  // Catmull-Rom spline for smooth lines
  const size_t n = points.size();
  for (size_t i = 0; i < n - 1; ++i) {
    const vec2f& p0 = (i == 0) ? vec2f() : points[i - 1];
    const vec2f& p1 = points[i];
    const vec2f& p2 = points[i + 1];
    const vec2f& p3 = (i + 2 < n) ? points[i + 2] : vec2f();
    for (int j = 0; j < CURVE_SEGMENTS; ++j) {
      const float t = static_cast<float>(j) * CURVE_SEGMENTS_INVERSE;
      vec2f pointA = catmullRom(p0, p1, p2, p3, t) * 2.0f; // Scale up because we're in NDC
      vec2f pointB = catmullRom(p0, p1, p2, p3, t + CURVE_SEGMENTS_INVERSE) * 2.0f;
      addSegment(pointA, pointB);
    }
  }
}

void Line::generateMeshData() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // Bind VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(vec2f)),
               &vertices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vec2f),
                        nullptr);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void Line::addSegment(const vec2f &start, const vec2f &end) {
  const vec2f direction = (end - start).normalized();
  const vec2f perpendicular = vec2f(-direction.y, direction.x) * 0.002f;

  if (vertices.empty()) {
    vertices.push_back(start + perpendicular);
    vertices.push_back(start - perpendicular);
  }
  const auto index = static_cast<unsigned int>(vertices.size());
  vertices.push_back(end + perpendicular);
  vertices.push_back(end - perpendicular);
}
