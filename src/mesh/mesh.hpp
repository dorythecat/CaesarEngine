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
    Color(std::string hex) {
      r = (unsigned char)std::stoi(hex.substr(0, 2), nullptr, 16);
      g = (unsigned char)std::stoi(hex.substr(2, 2), nullptr, 16);
      b = (unsigned char)std::stoi(hex.substr(4, 2), nullptr, 16);
    }
  };
  struct Vertex {
    float x, y, z;

    Vertex() : x(0.0f), y(0.0f), z(0.0f) {}
    Vertex(float x, float y) : x(x), y(y), z(0.0f) {}
    Vertex(float x, float y, float z) : x(x), y(y), z(z) {}
  };

  Mesh(const char* mapPath, Shader shader, Color color = Color(1.0f, 1.0f, 1.0f));
  ~Mesh() noexcept {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  Mesh(const Mesh& other) {
    vertices = other.vertices;
    indices = other.indices;
    shader = other.shader;
    color = other.color;

    generateMeshData(); // Regenerate the mesh, otherwise it won't render
  }

  void render();

private:
  unsigned int VAO, VBO, EBO;
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Shader shader = Shader("res/shaders/default.vert", "res/shaders/default.frag");
  Color color;

  void generateMesh(const char* mapPath);
  void generateMeshData();
};

#endif // MESH_HPP
