#include "province.hpp"

Province::Province(const char* mapPath,
                   Shader shader,
                   Color color,
                   std::string name) :
  shader(shader), color(color), name(name) {
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

  for (int i= 0; i < x * y * n; i += n) {
    if (data[i] != color.r ||
        data[i + 1] != color.g ||
        data[i + 2] != color.b) continue;
    int m = i / n;
    float p = (float)(m % x) / (float)x - 1.0f;
    float q = - (float)(m / x) / (float)y;

    addQuad(p, q, p + 1.0f / x, q - 1.0f / y, color);
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

  indices.push_back(vertices.size() - 4);
  indices.push_back(vertices.size() - 3);
  indices.push_back(vertices.size() - 2);

  indices.push_back(vertices.size() - 1);
  indices.push_back(vertices.size() - 2);
  indices.push_back(vertices.size() - 3);
}

void Province::render(bool useShader) {
  if (useShader) shader.use();
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,
                 (int)indices.size(),
                 GL_UNSIGNED_INT,
                 0);
  glBindVertexArray(0);
}

bool Province::clickedOn(double x, double y) {
  // There's probably a better way to do this, but it handles all edge cases and everything
  // for us, so...
  for (unsigned int i = 0; i < vertices.size(); i += 4) {
    Vertex v1 = vertices[i];
    Vertex v2 = vertices[i + 2];
    if (x >= v1.x && x <= v2.x &&
        y >= v2.y && y <= v1.y) return true;
  } return false;
}
