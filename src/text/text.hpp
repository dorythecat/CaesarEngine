#ifndef TEXT_HPP
#define TEXT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "../utils.hpp"
#include "../province/province.hpp"

struct Character {
  float advance;
  float quadLeft, quadBottom, quadRight, quadTop;
  float atlasLeft, atlasBottom, atlasRight, atlasTop;
};

class Text {
public:
  Text(std::string atlasPath, std::string indexPath);
  ~Text();

  void setText(std::string text, float x, float y, float scale, vec2i windowDimensions);
  void render();

private:
  std::vector<Character> characters;
  GLuint atlas, VAO, VBO, EBO;
  unsigned int size;
};

#endif // TEXT_HPP
