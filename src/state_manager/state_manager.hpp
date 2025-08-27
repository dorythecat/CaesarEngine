#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <string>
#include <map>
#include <memory>
#include <ranges>

#include "../utils.hpp"
#include "../window/window.hpp"
#include "../province/province.hpp"
#include "../province_manager/province_manager.hpp"
#include "../state/state.hpp"
#include "../text/text.hpp"
#include "../error_handler/error_handler.h"

class StateManager {
public:
  std::unique_ptr<ProvinceManager> pm;

  explicit StateManager(ErrorHandler* errorHandler,
                        const std::string &provShaderPath = "res/shaders/default",
                        const std::string &textShaderPath = "res/shaders/text",
                        const std::string &mapPath = "res/test.png",
                        const std::string &provPath = "res/provinces.txt",
                        const std::string &statePath = "res/states.txt");
  ~StateManager() = default;

  StateManager(const StateManager&) = delete;
  StateManager& operator=(const StateManager&) = delete;

  void render(const Window &window, float scale, const vec2f &offset);
  std::string clickedOnState(float x, float y) const;

  [[nodiscard]] Province& getProvince(const std::string& name) const { return pm->getProvince(name); }
  [[nodiscard]] State& getState(const std::string& name) { return states.at(name); }

private:
  std::map<std::string, State> states;
  Text text;
  ErrorHandler* errorHandler;
};

#endif // STATE_MANAGER_HPP
