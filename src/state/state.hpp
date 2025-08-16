#ifndef STATE_HPP
#define STATE_HPP

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../province/province.hpp"

class State {
public:
  explicit State(std::string name) : name(std::move(name)) {}
  ~State() = default;

  State(const State& other) {
    name = other.name;
    provinces = other.provinces;

    centerX = other.centerX;
    centerY = other.centerY;
  }

  void addProvince(const Province& province) {
    provinces.push_back(province);
    centerX += province.getCenterX();
    centerY += province.getCenterY();
  }
  void removeProvince(const std::string& provinceName) {
    for (auto it = provinces.begin(); it != provinces.end(); ++it) {
      if (it->getName() == provinceName) {
        provinces.erase(it);
        centerX -= it->getCenterX();
        centerY -= it->getCenterY();
        break;
      }
    }
  }
  [[nodiscard]] float getCenterX() const { return centerX / static_cast<float>(provinces.size()); }
  [[nodiscard]] float getCenterY() const { return centerY / static_cast<float>(provinces.size()); }

  [[nodiscard]] std::string getName() const { return name; }

  [[nodiscard]] bool hasProvince(const std::string &provinceName) const {
    std::ranges::any_of(provinces,
      [&](const Province& province) { return province.getName() == provinceName; });
    return false;
  }

private:
  std::string name;
  std::vector<Province> provinces;

  float centerX = 0, centerY = 0;
};

#endif // STATE_HPP
