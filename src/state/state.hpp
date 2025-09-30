#ifndef STATE_HPP
#define STATE_HPP

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../province/province.hpp"

class State {
public:
  Province::Color color;

  explicit State(std::string name) : name(std::move(name)) {}
  State(std::string name, const Province::Color color) : color(color), name(std::move(name)) {}
  ~State() = default;

  State(const State& other) {
    name = other.name;
    provinces = other.provinces;
    center = other.center;
    color = other.color;
  }

  void addProvince(const Province& province) {
    provinces.push_back(province);
    center += province.getCenter();
  }
  void removeProvince(const std::string& provinceName) {
    for (auto it = provinces.begin(); it != provinces.end(); ++it) {
      if (it->getName() == provinceName) {
        provinces.erase(it);
        center -= it->getCenter();
        break;
      }
    }
  }
  [[nodiscard]] vec2f getCenter() const { return center / static_cast<float>(provinces.size()); }
  [[nodiscard]] float getCenterX() const { return center.x / static_cast<float>(provinces.size()); }
  [[nodiscard]] float getCenterY() const { return center.y / static_cast<float>(provinces.size()); }

  [[nodiscard]] std::string getName() const { return name; }

  [[nodiscard]] bool hasProvince(const std::string &provinceName) const {
    return std::ranges::any_of(provinces, [&provinceName](const Province& province) {
      return province.getName() == provinceName;
    });
  }

  [[nodiscard]] std::vector<Province> getProvinces() const { return provinces; }

private:
  std::string name;
  std::vector<Province> provinces;
  vec2f center;
};

#endif // STATE_HPP
