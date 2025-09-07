#ifndef PROVINCE_MANAGER_HPP
#define PROVINCE_MANAGER_HPP

#include <string>
#include <map>
#include <unordered_set>
#include <ranges>

#include "../utils.hpp"
#include "../window/window.hpp"
#include "../shader/shader.hpp"
#include "../province/province.hpp"
#include "../text/text.hpp"
#include "../error_handler/error_handler.h"

class ProvinceManager {
public:
  struct QueuedProvince {
    std::string id;
    Province::Color color;
    std::string name;
    Province::City city;
  };

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

  [[nodiscard]] int connected(const std::string &provinceA, const std::string &provinceB) {
    if (provinceA == provinceB) return 0;
    if (!provinces.contains(provinceA) || !provinces.contains(provinceB)) return -1;

    std::unordered_set<std::string> visited;
    std::vector<std::string> toVisit = { provinceA };
    int depth = 0;

    while (!toVisit.empty()) {
      std::vector<std::string> nextToVisit;
      for (const auto& current : toVisit) {
        if (current == provinceB) return depth;
        visited.insert(current);
        for (const auto& neighbor : adjacencyMap[current]) {
          if (!visited.contains(neighbor))
            nextToVisit.push_back(neighbor);
        }
      }
      toVisit = nextToVisit;
      depth++;
    } return -1; // Not connected
  }

private:
  std::map<std::string, Province> provinces;
  Text text;
  ErrorHandler* errorHandler;

  std::map<std::string, std::vector<std::string>> adjacencyMap;
};

#endif // PROVINCE_MANAGER_HPP
