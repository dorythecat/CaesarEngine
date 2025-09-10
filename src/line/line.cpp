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
  for (size_t i = 0; i < n - 1; i++) {
    const vec2f& p0 = (i == 0) ? points[i].normalized() : points[i - 1];
    const vec2f& p1 = points[i];
    const vec2f& p2 = points[i + 1];
    const vec2f& p3 = (i + 2 < n) ? points[i + 2] : points[i + 1].normalized();
    for (int j = 0; j < CURVE_SEGMENTS; j++) {
      const float t = static_cast<float>(j) * CURVE_SEGMENTS_INVERSE;
      vec2f pointA = catmullRom(p0, p1, p2, p3, t) * 2.0f; // Scale up because we're in NDC
      vec2f pointB = catmullRom(p0, p1, p2, p3, t + CURVE_SEGMENTS_INVERSE) * 2.0f;
      addSegment(pointA, pointB, j == CURVE_SEGMENTS - 1 && i == n - 2);
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

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2f), nullptr);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void Line::addSegment(const vec2f &start, const vec2f &end, const bool final) {
  const vec2f direction = (end - start).normalized();
  const vec2f perpendicular = direction.perpendicular() * 0.002f;

  if (vertices.empty()) { // Only needed for the first segment
    vertices.push_back(start + perpendicular);
    vertices.push_back(start - perpendicular);
  }
  if (!final) {
    vertices.push_back(end + perpendicular);
    vertices.push_back(end - perpendicular);
    return;
  }

  // Final segment, add an arrow pointing to the final point
  vertices.push_back(end);
  vertices.push_back(end - direction * 0.01f + perpendicular * 2.0f);
  vertices.push_back(end);
  vertices.push_back(end - direction * 0.01f - perpendicular * 2.0f);
}

vec2f Line::catmullRom(const vec2f &p0, const vec2f &p1, const vec2f &p2, const vec2f &p3, float t) {
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
