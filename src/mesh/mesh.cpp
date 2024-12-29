#include "mesh.hpp"

/*
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Shader shader) :
  vertices(vertices), indices(indices), shader(shader) {
  generateMesh();
}
*/

Mesh::Mesh(const char* mapPath, Shader shader, Color color) :
  shader(shader), color(color) {
  int x, y, n;
  unsigned char* data = stbi_load(mapPath, &x, &y, &n, 0);
  if (!data) {
    std::cout << "Failed to load texture" << std::endl;
    return;
  }

  vertices.reserve(x * y * 4);
  indices.reserve(x * y * 6);

  unsigned int j = 0;
  for (int i= 0; i < x * y * n; i += n) {
    if ((n <= 2 && data[i] != color.r) ||
        data[i] != color.r ||
        data[i + 1] != color.g ||
        data[i + 2] != color.b) continue;
    int m = i / n;
    float p = (float)(m % x) / (float)x - 1.0f;
    float q = - (float)(m / x) / (float)y;

    vertices.push_back(Mesh::Vertex(p, q));
    vertices.push_back(Mesh::Vertex(p, q - 1.0f / (float)y));
    vertices.push_back(Mesh::Vertex(p + 1.0f / (float)x, q - 1.0f / (float)y));
    vertices.push_back(Mesh::Vertex(p + 1.0f / (float)x, q));

    indices.push_back(j);
    indices.push_back(j + 1);
    indices.push_back(j + 3);

    indices.push_back(j + 1);
    indices.push_back(j + 2);
    indices.push_back(j + 3);

    j += 4;
  } stbi_image_free(data);

  vertices.shrink_to_fit();
  indices.shrink_to_fit();

  generateMesh();
}

void Mesh::generateMesh() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Bind VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void Mesh::render() {
  shader.use();
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
