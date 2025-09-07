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
  struct QueuedProvince { // Queued province for batch generation
    std::string id;
    Province::Color color;
    std::string name;
    Province::City city;
  };

  struct Connection { // Connections in between provinces
    Province &start, &end; // Start and end provinces
    int steps = -1; // -1 if not connected, 0 if same province, >0 if connected

    // What provinces to traverse
    std::map<std::string, Province> provinces; // Should be ordered for consistency

    Connection(Province &start, Province &end) : start(start), end(end) {}
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
  [[nodiscard]] std::string clickedOnProvince(const vec2f &pos) { return clickedOnProvince(pos.x, pos.y); }

  [[nodiscard]] Province& getProvince(const std::string &name) { return provinces.at(name); }

  [[nodiscard]] std::vector<Province> getAllProvinces() const {
    std::vector<Province> provinceList;
    provinceList.reserve(provinces.size());
    for (const auto &province: provinces | std::views::values) provinceList.push_back(province);
    return provinceList;
  }
  [[nodiscard]] std::map<std::string, Province> getAllProvincesMap() const { return provinces; }
  [[nodiscard]] std::map<std::string, std::vector<std::string>> getAdjacencyMap() const { return adjacencyMap; }

  [[nodiscard]] Connection connected(const std::string &provinceA, const std::string &provinceB) {
    Connection connection(provinces.at(provinceA), provinces.at(provinceB));
    if (!provinces.contains(provinceA) || !provinces.contains(provinceB)) return connection;
    if (provinceA == provinceB) {
      connection.steps = 0;
      return connection;
    }
    std::map<std::string, Province> visited; // Provinces we've already visited
    std::map<std::string, Province> toVisit; // Provinces we need to visit
    toVisit.emplace(provinceA, provinces.at(provinceA));
    while (!toVisit.empty()) {
      std::map<std::string, Province> nextToVisit; // Provinces to visit next
      for (const auto& [name, prov] : toVisit) {
        visited.emplace(name, prov);
        for (const auto& adjProvName : adjacencyMap[name]) {
          if (visited.contains(adjProvName) || toVisit.contains(adjProvName)) continue;
          if (adjProvName == provinceB) {
            connection.steps = static_cast<int>(visited.size());
            connection.provinces = visited;
            connection.provinces.emplace(provinceB, provinces.at(provinceB));
            return connection;
          } nextToVisit.emplace(adjProvName, provinces.at(adjProvName));
        }
      } toVisit = nextToVisit;
    } return connection; // Not connected
  }

private:
  std::map<std::string, Province> provinces;
  Text text;
  ErrorHandler* errorHandler;

  std::map<std::string, std::vector<std::string>> adjacencyMap;
};

#endif // PROVINCE_MANAGER_HPP
