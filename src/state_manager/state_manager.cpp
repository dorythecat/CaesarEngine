#include "state_manager.hpp"

#include <ranges>

StateManager::StateManager(const std::string &provShaderPath,
                           const std::string &textShaderPath,
                           const std::string &mapPath,
                           const std::string &provPath,
                           const std::string &statePath) {
  this->pm = std::make_unique<ProvinceManager>(provShaderPath,
                                               textShaderPath,
                                               mapPath,
                                               provPath);

  std::ifstream stateFile("res/states.txt");
  if (!stateFile.is_open()) {
    // TODO(Dory): Proper error handling
    std::cerr << "FATAL ERROR: Could not open file \"res/states.txt\"" << std::endl;
    return;
  }

  for (std::string line; std::getline(stateFile, line);) {
    if (line.empty()) continue;
    line = line.substr(line.find_first_not_of(' '));
    if (line.substr(0, 1) == "#" || line.substr(line.find_last_not_of(' ')) != "{") continue;

    std::string id = line.substr(0, line.find_first_of(' '));

    std::istringstream lineStream(line);
    std::string name;
    bool provinceSearch = false;
    std::vector<std::string> provinceIds;
    for (std::string cur; std::getline(stateFile, cur);) {

      if (cur.empty()) continue;
      cur = cur.substr(cur.find_first_not_of(' '));
      std::string first = cur.substr(0, 1);
      if (first == "#") continue;
      if (provinceSearch) {
        if (first == "}") {
          provinceSearch = false;
          continue;
        }
        std::istringstream curStream(cur);
        for (std::string provinceId;
             std::getline(curStream, provinceId, ',');) {
          provinceId = provinceId.substr(provinceId.find_first_not_of(' '));
          if (provinceId == "}") {
            provinceSearch = false;
            break;
          }
          provinceIds.push_back(provinceId);
        }
      }
      if (first == "}") break;
      first = cur.substr(0, cur.find_first_of(' '));
      if (first == "name:") {
        name = cur.substr(cur.find(first) + first.length());
        name = name.substr(name.find_first_not_of(' '));
        name = name.substr(name.find_first_not_of('"'));
        name = name.substr(0, name.find_last_not_of('"') + 1);
      } else if (first == "provinces:") {
        provinceSearch = true;
      }
    }

    if (name.empty()) {
      std::cerr << "FATAL ERROR: State " << id << " has no name" << std::endl;
      return;
    }

    if (provinceIds.empty()) {
      std::cerr << "FATAL ERROR: State " << id << " has no provinces" << std::endl;
      return;
    }

    State state(name);
    for (const auto &provinceId: provinceIds) state.addProvince(pm->getProvince(provinceId));
    states.emplace(id, state);
  }
  stateFile.close();

  if (states.empty()) {
    std::cerr << "FATAL ERROR: No states found in \"res/states.txt\"" << std::endl;
    return;
  }
}

void StateManager::render(Window &window, const float scale, const vec2f &offset) {
  pm->render(window, scale, offset);

  pm->textShader.use();
  for (auto &[name, state]: states) {
    text.setText(name, 10.0f, state.getCenter(), static_cast<vec2f>(window.getDimensions()));
    text.render();
  }
}

std::string StateManager::clickedOnState(float x, float y) const {
  const std::string provinceName = pm->clickedOnProvince(x, y);
  if (provinceName.empty()) return "";
  const Province province = pm->getProvince(provinceName);
  for (const auto &state: states | std::views::values)
    if (state.hasProvince(province.getName())) return state.getName();
  std::cerr << "FATAL ERROR: Province " << provinceName << " not found in any state" << std::endl;
  return "";
}
