#include "line.h"

Line::Line(ErrorHandler* errorHandler,
           std::vector<vec2f> points) : errorHandler(errorHandler) {
  generateMesh(std::move(points));
  generateMeshData();
}

void Line::render(const Window &window, float scale, const vec2f &offset) const {
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES,
                 static_cast<int>(indices.size()),
                 GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

void Line::generateMesh(std::vector<vec2f> points) {
  if (points.size() < 2) {
    errorHandler->logError("Line must have at least 2 points");
    return;
  }

  // Very, very basic line generation
  vertices = std::move(points);
  indices.reserve((vertices.size() - 1) * 2);
  for (unsigned int i = 0; i < vertices.size() - 1; i++) {
    indices.push_back(i);
    indices.push_back(i + 1);
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
