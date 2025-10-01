#ifndef STATE_HPP
#define STATE_HPP

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "../province/province.hpp"

class State {
public:
  explicit State(std::string name) : name(std::move(name)) { checkColor(); }
  State(std::string name, const Province::Color color) : name(std::move(name)), color(color) { checkColor(); }
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
  [[nodiscard]] Province::Color getColor() const { return color; }

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

  Province::Color color;

  void checkColor() {
    if (this->color != Province::Color()) return;
    // If no color is given, generate one based on the name
    unsigned int hash = 196458761; // Random, but constant starting value
    for (const char &c: this->name) hash = (hash << 3) + (hash >> 3) + static_cast<unsigned int>(c) * hash;
    this->color = Province::Color((hash & 0xFF0000) >> 16, (hash & 0x00FF00) >> 8, hash & 0x0000FF);
  }
};

#endif // STATE_HPP
