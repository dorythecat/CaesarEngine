#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include "../utils.hpp"
#include "../window/window.hpp"
#include "../province/province.hpp"
#include "../province_manager/province_manager.hpp"
#include "../state/state.hpp"
#include "../text/text.hpp"

class StateManager {
public:
  std::unique_ptr<ProvinceManager> pm;

  StateManager(std::string provShaderFile = "res/shaders/default",
               std::string textShaderFile = "res/shaders/text",
               std::string mapPath = "res/test.png",
               std::string provPath = "res/provinces.txt",
               std::string statePath = "res/states.txt");
  ~StateManager() = default;

  StateManager(const StateManager&) = delete;
  StateManager& operator=(const StateManager&) = delete;

  void render(Window &window, float scale, vec2f offset);
  std::string clickedOnState(float x, float y);

  Province& getProvince(const std::string& name) {
    return pm->getProvince(name);
  }
  State& getState(const std::string& name) {
    return states.at(name);
  }

private:
  std::map<std::string, State> states;
  Text text;
};

#endif // STATE_MANAGER_HPP
