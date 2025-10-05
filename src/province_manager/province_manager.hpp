#ifndef PROVINCE_MANAGER_HPP
#define PROVINCE_MANAGER_HPP

#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <ranges>
#include <list>
#include <vector>
#include <queue>

#include "../utils.hpp"
#include "../window/window.hpp"
#include "../shader/shader.hpp"
#include "../province/province.hpp"
#include "../text/text.hpp"
#include "../error_handler/error_handler.h"
#include "../line/line.h"

class ProvinceManager {
public:
  struct QueuedProvince { // Queued province for batch generation
    std::string id;
    Province::Color color;
    std::string name;
    Province::City city;
  };

  struct Connection { // Connections in between provinces
    int steps = -1; // -1 if not connected, 0 if same province, >0 if connected
    float length = 0.0f; // Total length of the path

    // What provinces to traverse
    std::list<std::pair<std::string, Province>> provinces; // Should be ordered for consistency

    bool operator==(const Connection &other) const { return steps == other.steps && provinces == other.provinces; }
  };

  Shader provShader, textShader, lineShader;

  explicit ProvinceManager(ErrorHandler* errorHandler,
                           const std::string& provShaderPath = "res/shaders/default",
                           const std::string& textShaderPath = "res/shaders/text",
                           const std::string& lineShaderPath = "res/shaders/line",
                           const std::string& mapPath = "res/test.png",
                           const std::string& provPath = "res/provinces.txt");
  ~ProvinceManager() = default;

  ProvinceManager(const ProvinceManager&) = delete;
  ProvinceManager& operator=(const ProvinceManager&) = delete;

  void render(const Window &window,
              float scale,
              const vec2f &offset,
              const std::unordered_map<std::string, Province::Color> &provColors);
  [[nodiscard]] std::string clickedOnProvince(float x, float y);
  [[nodiscard]] std::string clickedOnProvince(const vec2f& pos) { return clickedOnProvince(pos.x, pos.y); }

  [[nodiscard]] Province& getProvince(const std::string& name) { return provinces.at(name); }

  [[nodiscard]] std::vector<Province> getAllProvinces() const {
    std::vector<Province> provinceList;
    provinceList.reserve(provinces.size());
    for (const auto &province: provinces | std::views::values) provinceList.push_back(province);
    return provinceList;
  }
  [[nodiscard]] std::map<std::string, Province> getAllProvincesMap() const { return provinces; }
  [[nodiscard]] std::map<std::string, std::unordered_set<std::string>> getAdjacencyMap() const { return adjacencyMap; }

  [[nodiscard]] Connection findPath(const std::string& provinceA, const std::string& provinceB);

  void tick() { for (auto& province : provinces | std::views::values) province.tick(); }

private:
  std::map<std::string, Province> provinces;
  Text text;
  ErrorHandler* errorHandler;
  Line line; // For debugging paths

  std::map<std::string, std::unordered_set<std::string>> adjacencyMap;
};

#endif // PROVINCE_MANAGER_HPP
