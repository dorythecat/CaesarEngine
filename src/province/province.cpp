#include "province.hpp"

Province::Province(const char* mapPath,
                   Color color,
                   std::string name) :
  color(color), name(name) {
  generateMesh(mapPath);
  generateMeshData();
}

void Province::generateMesh(const char* mapPath) {
  int x, y, n;
  unsigned char* data = stbi_load(mapPath, &x, &y, &n, 0);
  if (!data) {
    std::cout << "Failed to load texture" << std::endl;
    return;
  }

  vertices.reserve(x * y * 4);
  indices.reserve(x * y * 6);

  for (int i = 0; i < x * y * n; i += n) {
    if (data[i] != color.r ||
        data[i + 1] != color.g ||
        data[i + 2] != color.b) continue;
    float x1 = 1.0f / (float)x;
    float y1 = -1.0f / (float)y;

    int xy = i / n;
    float p = (float)(xy % x) * x1 - 1.0f;
    float q = (float)(xy / x) * y1;

    float p0 = p;
    while (data[i] == color.r &&
           data[i + 1] == color.g &&
           data[i + 2] == color.b &&
           i % (n * x) != 0) {
      p0 += x1;
      i += n;
    }

    addQuad(p, q, p0, q + y1, color);
  } stbi_image_free(data);

  // Shrink to fit, so we don't waste memory
  vertices.shrink_to_fit();
  indices.shrink_to_fit();
}

void Province::generateMeshData() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Bind VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(Vertex),
               &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               indices.size() * sizeof(unsigned int),
               &indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,
                        3,
                        GL_UNSIGNED_BYTE,
                        GL_FALSE,
                        sizeof(Vertex),
                        (void*)offsetof(Vertex, color));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Province::addQuad(float x0, float y0, float x1, float y1, Color c) {
  vertices.push_back(Vertex(x0, y0, c));
  vertices.push_back(Vertex(x0, y1, c));
  vertices.push_back(Vertex(x1, y0, c));
  vertices.push_back(Vertex(x1, y1, c));

  unsigned int offset = (unsigned int)vertices.size() - 4;

  indices.push_back(offset);
  indices.push_back(offset + 1);
  indices.push_back(offset + 2);

  indices.push_back(offset + 3);
  indices.push_back(offset + 2);
  indices.push_back(offset + 1);
}

void Province::render() {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,
                 (int)indices.size(),
                 GL_UNSIGNED_INT,
                 0);
  glBindVertexArray(0);
}

bool Province::clickedOn(float x, float y) {
  // There's probably a better way to do this, but it handles all edge cases and everything
  // for us, so...
  for (unsigned int i = 0; i < vertices.size(); i += 4) {
    Vertex v1 = vertices[i];
    Vertex v2 = vertices[i + 3];
    if (x >= v1.x && x <= v2.x &&
        y >= v2.y && y <= v1.y) return true;
  } return false;
}
