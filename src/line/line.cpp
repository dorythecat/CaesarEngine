#include "line.h"

Line::Line(ErrorHandler* errorHandler,
           const std::vector<vec2f> &points) : errorHandler(errorHandler) {
  generateMesh(points);
  generateMeshData();
}

void Line::render() const {
  std::cout << "Rendering line with " << indices.size() / 3 << " triangles." << std::endl;
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<GLint>(indices.size()), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Line::generateMesh(const std::vector<vec2f> &points) {
  if (points.size() < 2) {
    errorHandler->logError("Line must have at least 2 points");
    return;
  }

  // Very, very basic line generation
  for (size_t i = 0; i < points.size() - 1; ++i) {
    addSegment(points[i], points[i + 1]);
  }
}

void Line::generateMeshData() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Bind VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(vec2f)),
               &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
               &indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(vec2f),
                        nullptr);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Line::addSegment(const vec2f &start, const vec2f &end) {
  // Very basic line segment addition (not handling joins or anything)

  const auto index = static_cast<unsigned int>(vertices.size());

  vertices.push_back(start - 0.05f);
  vertices.push_back(start + 0.05f);
  vertices.push_back(end - 0.05f);
  vertices.push_back(end + 0.05f);

  indices.push_back(index);
  indices.push_back(index + 1);
  indices.push_back(index + 2);

  indices.push_back(index + 3);
  indices.push_back(index + 2);
  indices.push_back(index + 1);
}
