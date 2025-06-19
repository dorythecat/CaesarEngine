#include "state_manager.hpp"

StateManager::StateManager(std::string provShaderPath,
                           std::string textShaderPath,
                           std::string mapPath,
                           std::string provPath,
                           std::string statePath) {
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

  unsigned int i = 0;
  for (std::string line; std::getline(stateFile, line);) {
    i++;
    if (line.empty()) continue;
    line = line.substr(line.find_first_not_of(' '));
    std::string first = line.substr(0, 1);
    std::string last = line.substr(line.find_last_not_of(' '));
    if (first == "#" || last != "{") continue;

    std::string id = line.substr(0, line.find_first_of(' '));

    std::istringstream lineStream(line);
    std::string name;
    bool provinceSearch = false;
    std::vector<std::string> provinceIds;
    for (std::string cur; std::getline(stateFile, cur);) {
      i++;
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
    for (const auto &provinceId : provinceIds)
      state.addProvince(pm->getProvince(provinceId));
    states.emplace(id, state);
  } stateFile.close();

  if (states.empty()) {
    std::cerr << "FATAL ERROR: No states found in \"res/states.txt\"" << std::endl;
    return;
  }
}

void StateManager::render(Window &window, float scale, vec2f offset) {
  pm->render(window, scale, offset);

  pm->textShader.use();
  for (auto &state : states) {
    vec2i dimensions = window.getDimensions();
      text.setText(state.first,
                   (state.second.getCenterX() + 0.5f) * static_cast<float>(dimensions.x) - 10.0f,
                   (state.second.getCenterY() + 0.5f) * static_cast<float>(dimensions.y),
                   10.0f,
                   dimensions);
      text.render();
  }
}

std::string StateManager::clickedOnState(float x, float y) {
  std::string provinceName = pm->clickedOnProvince(x, y);
  if (provinceName == "") return "";
  Province province = pm->getProvince(provinceName);
  for (auto &state : states) {
    if (state.second.hasProvince(province.getName())) return state.second.getName();
  }
  std::cerr << "FATAL ERROR: Province " << provinceName
            << " not found in any state" << std::endl;
  return "";
}
