#ifndef PROVINCE_HPP
#define PROVINCE_HPP

#include <glad/glad.h>

#include <string>
#include <vector>
#include <unordered_set>

#include <stb_image.h>

#include "../shader/shader.hpp"
#include "../utils.hpp"
#include "../error_handler/error_handler.h"

class Province {
public:
  struct Color {
    unsigned char r, g, b;

    Color() : r(0.0f), g(0.0f), b(0.0f) {}
    explicit Color(const unsigned char grey) : r(grey), g(grey), b(grey) {}
    Color(const unsigned char r, const unsigned char g, const unsigned char b) : r(r), g(g), b(b) {}
    explicit Color(const std::string &hex) {
      r = static_cast<unsigned char>(std::stoi(hex.substr(0, 2), nullptr, 16));
      g = static_cast<unsigned char>(std::stoi(hex.substr(2, 2), nullptr, 16));
      b = static_cast<unsigned char>(std::stoi(hex.substr(4, 2), nullptr, 16));
    }

    // For unordered_set
    bool operator==(const Color& other) const { return r == other.r && g == other.g && b == other.b; }

    struct HashFunction {
      size_t operator()(const Color &aColor) const {
        const size_t rHash = std::hash<int>()(aColor.r);
        const size_t gHash = std::hash<int>()(aColor.g) << 1;
        const size_t bHash = std::hash<int>()(aColor.b) << 2;
        return rHash ^ gHash ^ bHash;
      }
    };
  };
  struct Vertex {
    float x, y;
    Color color;

    Vertex() : x(0.0f), y(0.0f) {}
    Vertex(const float x, const float y) : x(x), y(y) {}
    Vertex(const float x, const float y, const Color color) : x(x), y(y), color(color) {}

    // We don't need to compare the color, really (in fact, it benefits us not to)
    bool operator==(const Vertex& other) const { return x == other.x && y == other.y; }
  };

  struct City { // City wrapper
    enum CityCategory {
      SINGLE_PROVINCE_CAPITAL = 0,
      MULTI_PROVINCE_CAPITAL = 1,
      CITY = 2,
      TOWN = 3,
      VILLAGE = 4,
      SETTLEMENT = 5,
      UNASSIGNED_START = 6, // Inclusive
      UNASSIGNED_END = 253, // Inclusive
      NO_CITY = 254, // Effectively the same as wasteland, at least as of now
      WASTELAND = 255 // Untraversable, can't be part of a state
    };

    CityCategory category;
    int population, wealth, food, production, strength;
    ErrorHandler* errorHandler;

    explicit City(ErrorHandler* errorHandler, CityCategory category = WASTELAND) :
    category(category), errorHandler(errorHandler) {
      // TODO(Dory): Properly assign default values and state scaling
      switch (category) { // Assign default values based on category
        case SINGLE_PROVINCE_CAPITAL:
          population = 10000;
          wealth = 10000;
          food = 10000;
          production = 10000;
          strength = 10000;
          break;
        case MULTI_PROVINCE_CAPITAL:
          population = 10000;
          wealth = 10000;
          food = 10000;
          production = 10000;
          strength = 10000;
          break;
        case CITY:
          population = 10000;
          wealth = 10000;
          food = 10000;
          production = 10000;
          strength = 10000;
          break;
        case TOWN:
          population = 10000;
          wealth = 10000;
          food = 10000;
          production = 10000;
          strength = 10000;
          break;
        case VILLAGE:
          population = 10000;
          wealth = 10000;
          food = 10000;
          production = 10000;
          strength = 10000;
          break;
        case SETTLEMENT:
          population = 10000;
          wealth = 10000;
          food = 10000;
          production = 10000;
          strength = 10000;
          break;
        default: // Unassigned (between UNASSIGNED_START and UNASSIGNED_END, both inclusive)
          errorHandler->logError("Province was assigned an unassigned city category, assigning wasteland",
            ErrorHandler::UNASSIGNED_PROVINCE_CATEGORY);
          category = WASTELAND; // Set category to wasteland to avoid future issues
          [[fallthrough]];
          // We zero out everything for NO_CITY and WASTELAND, as well as for unassigned province categories
        case NO_CITY:
        case WASTELAND:
          population = 0;
          wealth = 0;
          food = 0;
          production = 0;
          strength = 0;
          break;
      }
    }
  };

  City city; // The city inside this province
  Province(ErrorHandler* errorHandler,
           const char* mapPath,
           Color color,
           std::string name,
           const City &city,
           const std::unordered_set<Color, Color::HashFunction> &usedColors);
  ~Province() noexcept {
    // Clean up the mesh data
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  Province(const Province& other) : city(other.city) {
    name = other.name;

    vertices = other.vertices;
    indices = other.indices;
    color = other.color;

    center = other.center;
    area = other.area;

    adjacentColors = other.adjacentColors;

    errorHandler = other.errorHandler;

    generateMeshData(); // Regenerate the mesh data, otherwise it won't render
  }

  void render() const;

  [[nodiscard]] bool clickedOn(float x, float y) const;

  [[nodiscard]] std::string getName() const { return name; }
  [[nodiscard]] Color getColor() const { return color; }
  [[nodiscard]] vec2f getCenter() const { return center; }
  [[nodiscard]] float getCenterX() const { return center.x; }
  [[nodiscard]] float getCenterY() const { return center.y; }

  [[nodiscard]] bool isAdjacent(const Color c) const { return adjacentColors.contains(c); }
  [[nodiscard]] bool isAdjacent(Province* p) const { return isAdjacent(p->getColor()); }

  [[nodiscard]] std::vector<Color> getAdjacentColors() const {
    return { adjacentColors.begin(), adjacentColors.end() };
  }
  [[nodiscard]] std::unordered_set<Color, Color::HashFunction> getAdjacentColorsSet() const { return adjacentColors; }

private:
  unsigned int VAO{}, VBO{}, EBO{};
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Color color;
  std::string name;
  vec2f center;
  int area = 0; // The area of this province, in number of pixels

  std::unordered_set<Color, Color::HashFunction> adjacentColors;

  ErrorHandler* errorHandler;

  void generateMesh(const char* mapPath, std::unordered_set<Color, Color::HashFunction> usedColors);
  void generateMeshData();
  void addQuad(float x0, float y0, float x1, float y1, Color c);
};

#endif // PROVINCE_HPP
