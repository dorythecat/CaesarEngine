#ifndef TEXT_HPP
#define TEXT_HPP

#include <glad/glad.h>

#include <string>
#include <vector>
#include <fstream>

#include "../utils.hpp"
#include "../province/province.hpp" // Include for stb_image
#include "../error_handler/error_handler.h"

struct Character {
  float advance;
  float quadLeft, quadBottom, quadRight, quadTop;
  float atlasLeft, atlasBottom, atlasRight, atlasTop;
};

class Text {
public:
  explicit Text(ErrorHandler* errorHandler,
                const std::string &atlasPath = "res/text.png",
                const std::string &indexPath = "res/text.csv");
  ~Text();

  void setText(const std::string &text, float scale, vec2f position, const vec2f &windowDimensions, const vec2f &offset);
  void render() const;

private:
  std::vector<Character> characters;
  GLuint atlas{}, VAO{}, VBO{}, EBO{};
  unsigned int size{};
  ErrorHandler* errorHandler;
};

#endif // TEXT_HPP
