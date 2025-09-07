#ifndef PROVINCE_MANAGER_HPP
#define PROVINCE_MANAGER_HPP

#include <algorithm>
#include <string>
#include <map>
#include <unordered_set>
#include <ranges>
#include <queue>
#include <list>
#include <vector>

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
    std::list<std::pair<std::string, Province>> provinces; // Should be ordered for consistency

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
  [[nodiscard]] std::map<std::string, std::unordered_set<std::string>> getAdjacencyMap() const { return adjacencyMap; }

  [[nodiscard]] Connection connected(const std::string &provinceA, const std::string &provinceB) {
    Connection connection(provinces.at(provinceA), provinces.at(provinceB));
    if (!provinces.contains(provinceA) || !provinces.contains(provinceB)) return connection;
    if (provinceA == provinceB) {
      connection.steps = 0;
      return connection;
    }

    // Dijkstra's algorithm
    std::queue<std::string> toVisit;
    std::unordered_set<std::string> visited;
    std::list<std::pair<std::string, Province>> output;
    toVisit.push(provinceA); // Start from province A
    while (!toVisit.empty()) {
      std::string cur = toVisit.front();
      toVisit.pop();
      visited.insert(cur);
      output.emplace_back(cur, provinces.at(cur));
      std::unordered_set<std::string> adjacenciesSet = adjacencyMap.at(cur);
      std::vector<std::string> adjacencies(adjacenciesSet.begin(), adjacenciesSet.end());
      std::ranges::sort(adjacencies, [this](const std::string &a, const std::string &b) {
        return provinces.at(a).getArea() < provinces.at(b).getArea(); // Sort from least to most area
      });
      for (const auto &adj : adjacencies) {
        if (visited.contains(adj)) continue;
        if (adj == provinceB) {
          connection.steps = static_cast<int>(output.size());
          output.emplace_back(adj, provinces.at(adj));
          connection.provinces = output;
          return connection;
        } toVisit.push(adj);
      }
    } return connection;
  }

private:
  std::map<std::string, Province> provinces;
  Text text;
  ErrorHandler* errorHandler;

  std::map<std::string, std::unordered_set<std::string>> adjacencyMap;
};

#endif // PROVINCE_MANAGER_HPP
