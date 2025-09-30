#include "state_manager.hpp"

StateManager::StateManager(ErrorHandler* errorHandler,
                           const std::string& provShaderPath,
                           const std::string& textShaderPath,
                           const std::string& lineShaderPath,
                           const std::string& mapPath,
                           const std::string& provPath,
                           const std::string& statePath) : text(errorHandler), errorHandler(errorHandler) {
  this->pm = std::make_unique<ProvinceManager>(errorHandler,
                                               provShaderPath,
                                               textShaderPath,
                                               lineShaderPath,
                                               mapPath,
                                               provPath);

  std::ifstream stateFile(statePath);
  if (!stateFile.is_open())
    errorHandler->logFatal("Could not open file \"" + statePath + "\"",
      ErrorHandler::COULD_NOT_OPEN_FILE_ERROR);

  for (std::string line; std::getline(stateFile, line);) {
    if (line.empty()) continue;
    line = line.substr(line.find_first_not_of(' '));
    if (line.substr(0, 1) == "#" || line.substr(line.find_last_not_of(' ')) != "{") continue;

    std::string id = line.substr(0, line.find_first_of(' '));
    if (id.empty()) {
      errorHandler->logWarning("State with no ID found", ErrorHandler::FORMAT_ERROR);
      continue;
    }
    if (states.contains(id)) {
      errorHandler->logWarning("State with ID " + id + " already exists", ErrorHandler::FORMAT_ERROR);
      continue;
    }
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
        for (std::string provinceId; std::getline(curStream, provinceId, ',');) {
          provinceId = provinceId.substr(provinceId.find_first_not_of(' '));
          if (provinceId == "}") {
            provinceSearch = false;
            break;
          } provinceIds.push_back(provinceId);
        }
      } if (first == "}") break;
      first = cur.substr(0, cur.find_first_of(' '));
      if (first == "name:") {
        name = cur.substr(cur.find(first) + first.length());
        name = name.substr(name.find_first_not_of(' '));
        name = name.substr(name.find_first_not_of('"'));
        name = name.substr(0, name.find_last_not_of('"') + 1);
      } else if (first == "provinces:") provinceSearch = true;
      else if (first == "color:") { // Ignore color for now
        errorHandler->logWarning("State " + id + " has a color, but state colors are not supported (yet)",
          ErrorHandler::FORMAT_ERROR);
      } else {
        errorHandler->logWarning("State " + id + " has an unknown parameter \"" + first + "\"",
          ErrorHandler::FORMAT_ERROR);
      }
    }

    if (name.empty()) {
      errorHandler->logWarning("State " + id + " has no name", ErrorHandler::FORMAT_ERROR);
      continue;
    }

    if (provinceIds.empty()) {
      errorHandler->logError("State " + id + " has no provinces", ErrorHandler::FORMAT_ERROR);
      continue;
    }

    State state(name);
    for (const auto &provinceId: provinceIds) state.addProvince(pm->getProvince(provinceId));
    states.emplace(id, state);
  } stateFile.close();

  if (states.empty())
    errorHandler->logFatal("No states found in \"" + statePath + "\"", ErrorHandler::FORMAT_ERROR);
}

void StateManager::render(const Window &window, const float scale, const vec2f &offset) {
  pm->render(window, scale, offset,
{
    { "PR1", Province::Color(255, 0, 0) },
    { "PR2", Province::Color(0, 255, 0) },
    { "PR3", Province::Color(0, 0, 255) },
    { "PR4", Province::Color(255, 0, 255) },
    { "PR5", Province::Color(255, 255, 0) },
    { "PR6", Province::Color(0, 255, 255) }
    });

  pm->textShader.use();
  if (scale < 0.15f || scale > 2.0f) return; // Don't render text if zoomed in too close or too far
  for (auto &[name, state]: states) {
    text.setText(name, 10.0f, state.getCenter(), static_cast<vec2f>(window.getDimensions()), offset);
    text.render();
  }
}

std::string StateManager::clickedOnState(const float x, const float y) const {
  const std::string provinceName = pm->clickedOnProvince(x, y);
  if (provinceName.empty()) return "";
  const Province province = pm->getProvince(provinceName);
  for (const auto &state: states | std::views::values)
    if (state.hasProvince(province.getName())) return state.getName();
  errorHandler->logError("Province " + provinceName + " not found in any state", ErrorHandler::UNKNOWN_ERROR);
  return "";
}
