#ifndef TEXT_HPP
#define TEXT_HPP

#include <glad/glad.h>

#include <string>
#include <vector>
#include <fstream>

#include "../utils.hpp"
#include "../province/province.hpp" // Include for stb_image

struct Character {
  float advance;
  float quadLeft, quadBottom, quadRight, quadTop;
  float atlasLeft, atlasBottom, atlasRight, atlasTop;
};

class Text {
public:
  explicit Text(const std::string& atlasPath = "res/text.png",
                const std::string& indexPath = "res/text.csv");
  ~Text();

  void setText(const std::string &text, float x, float y, float scale, const vec2i &windowDimensions);
  void render() const;

private:
  std::vector<Character> characters;
  GLuint atlas{}, VAO{}, VBO{}, EBO{};
  unsigned int size{};
};

#endif // TEXT_HPP
