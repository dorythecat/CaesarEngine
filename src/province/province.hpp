#ifndef PROVINCE_HPP
#define PROVINCE_HPP

#include <glad/glad.h>

#include <vector>

#include <stb_image.h>

#include "../shader/shader.hpp"

class Province {
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
    float x, y;
    float u, v;
    Color color;

    Vertex() : x(0.0f), y(0.0f), u(0.0f), v(0.0f) {}
    Vertex(float x, float y, float u, float v) : x(x), y(y), u(u), v(v) {}
    Vertex(float x, float y, float u, float v, Color color) : x(x), y(y), u(u), v(v), color(color) {}

    bool operator==(const Vertex& other) {
      // We don't need to compare the color, really (in fact, it benefits us not to)
      return x == other.x && y == other.y && u == other.u && v == other.v;
    }
  };

  Province(const char* mapPath,
           Color color,
           std::string name);
  ~Province() noexcept {
    // Clean up the mesh data
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  Province(const Province& other) {
    name = other.name;

    vertices = other.vertices;
    indices = other.indices;
    color = other.color;

    generateMeshData(); // Regenerate the mesh data, otherwise it won't render
  }

  void render();

  bool clickedOn(float x, float y);

private:
  unsigned int VAO, VBO, EBO;
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Color color;
  std::string name;

  void generateMesh(const char* mapPath);
  void generateMeshData();
  void addQuad(float x0, float y0, float u0, float v0,
               float x1, float y1, float u1, float v1,
               Color c);
};

#endif // PROVINCE_HPP
