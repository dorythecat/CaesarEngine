#ifndef STATE_HPP
#define STATE_HPP

#include <iostream>
#include <string>
#include <vector>

#include "../province/province.hpp"

class State {
public:
  State(const std::string name) : name(name) {}
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
  float getCenterX() const { return centerX / provinces.size(); }
  float getCenterY() const { return centerY / provinces.size(); }

private:
  std::string name;
  std::vector<Province> provinces;

  float centerX, centerY;
};

#endif // STATE_HPP
