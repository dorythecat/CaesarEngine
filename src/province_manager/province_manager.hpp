#ifndef PROVINCE_MANAGER_HPP
#define PROVINCE_MANAGER_HPP

#include <string>
#include <map>
#include <ranges>

#include "../utils.hpp"
#include "../window/window.hpp"
#include "../shader/shader.hpp"
#include "../province/province.hpp"
#include "../text/text.hpp"
#include "../error_handler/error_handler.h"

class ProvinceManager {
public:
  Shader provShader, textShader;

  explicit ProvinceManager(ErrorHandler* errorHandler,
                           const std::string &provShaderPath = "res/shaders/default",
                           const std::string &textShaderPath = "res/shaders/text",
                           const std::string &mapPath = "res/test.png",
                           const std::string &provPath = "res/provinces.txt");
  ~ProvinceManager() = default;

  ProvinceManager(const ProvinceManager&) = delete;
  ProvinceManager& operator=(const ProvinceManager&) = delete;

  void render(const Window &window, float scale, const vec2f &offset);
  [[nodiscard]] std::string clickedOnProvince(float x, float y);
  [[nodiscard]] std::string clickedOnProvince(const vec2f& pos) { return clickedOnProvince(pos.x, pos.y); }

  [[nodiscard]] Province& getProvince(const std::string &name) { return provinces.at(name); }

  [[nodiscard]] std::vector<Province> getAllProvinces() const {
    std::vector<Province> provinceList;
    provinceList.reserve(provinces.size());
    for (const auto &province: provinces | std::views::values) provinceList.push_back(province);
    return provinceList;
  }
  [[nodiscard]] std::map<std::string, Province> getAllProvincesMap() const { return provinces; }
  [[nodiscard]] std::map<std::string, std::vector<std::string>> getAdjacencyMap() const { return adjacencyMap; }

private:
  std::map<std::string, Province> provinces;
  Text text;
  ErrorHandler* errorHandler;

  std::map<std::string, std::vector<std::string>> adjacencyMap;
};

#endif // PROVINCE_MANAGER_HPP
