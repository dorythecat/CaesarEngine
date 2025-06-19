#ifndef PROVINCE_MANAGER_HPP
#define PROVINCE_MANAGER_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

#include "../utils.hpp"
#include "../window/window.hpp"
#include "../shader/shader.hpp"
#include "../province/province.hpp"
#include "../text/text.hpp"

class ProvinceManager {
public:
  Shader provShader;
  Shader textShader;

  ProvinceManager(std::string provShaderPath = "res/shaders/default",
                  std::string textShaderPath = "res/shaders/text",
                  std::string mapPath = "res/test.png",
                  std::string provPath = "res/provinces.txt");
  ~ProvinceManager() = default;

  ProvinceManager(const ProvinceManager&) = delete;
  ProvinceManager& operator=(const ProvinceManager&) = delete;

  void render(Window &window, float scale, vec2f offset);
  std::string clickedOnProvince(float x, float y);

  Province& getProvince(std::string name);

private:
  std::map<std::string, Province> provinces;
  Text text;
};

#endif // PROVINCE_MANAGER_HPP
