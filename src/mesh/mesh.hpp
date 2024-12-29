#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>

#include <vector>

#include <stb_image.h>

#include "../shader/shader.hpp"

class Mesh {
public:
  struct Color {
    unsigned char r, g, b;

    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    Color(unsigned char grey) : r(grey), g(grey), b(grey) {}
    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
  };
  struct Vertex {
    float x, y, z;

    Vertex() : x(0.0f), y(0.0f), z(0.0f) {}
    Vertex(float x, float y) : x(x), y(y), z(0.0f) {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
  };

  Mesh(const char* mapPath, Shader shader, Color color = Color(1.0f, 1.0f, 1.0f));
  // Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Shader shader);
  ~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  void generateMesh();
  void render();

private:
  unsigned int VAO, VBO, EBO;
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Shader shader;
  Color color;
};

#endif // MESH_HPP
