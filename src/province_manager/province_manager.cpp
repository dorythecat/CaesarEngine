#include "province_manager.hpp"

#include "../province/province.hpp"

ProvinceManager::ProvinceManager(ErrorHandler* errorHandler,
                                 const std::string &provShaderPath,
                                 const std::string &textShaderPath,
                                 const std::string& mapPath,
                                 const std::string& provPath) : provShader(errorHandler, provShaderPath),
                                                                textShader(errorHandler, textShaderPath),
                                                                text(errorHandler),
                                                                errorHandler(errorHandler) {
  std::ifstream province_file(provPath);
  if (!province_file.is_open())
    errorHandler->logFatal("Could not open file \"" + provPath + "\"", ErrorHandler::COULD_NOT_OPEN_FILE_ERROR);

  unsigned int i = 0; // Line number
  std::vector<QueuedProvince> queuedProvinces; // Read the provinces, queue them, and then generate them
  std::unordered_set<Province::Color, Province::Color::HashFunction> usedColors; // For adjacency optimizations
  for (std::string line; std::getline(province_file, line);) {
    i++;
    if (line.empty()) continue;
    line = line.substr(line.find_first_not_of(' '));
    if (line.substr(0, 1) == "#") continue; // Check for comments

    std::istringstream lineStream(line);
    std::vector<std::string> curProv;
    for (std::string cur; std::getline(lineStream, cur, ',');) {
      cur = cur.substr(cur.find_first_not_of(' '));
      curProv.push_back(cur);
    }

    if (curProv.size() < 4) {
      errorHandler->logWarning("Province defined at line " + std::to_string(i) + " lacks required information.",
        ErrorHandler::FORMAT_ERROR);
      continue;
    }

    if (curProv.size() > 9) {
      errorHandler->logWarning("Province defined at line " + std::to_string(i) + " has too many parameters.",
        ErrorHandler::FORMAT_ERROR);
      return;
    }

    auto city = Province::City(errorHandler, static_cast<Province::City::CityCategory>(std::stoi(curProv[3])));
    if (curProv.size() >= 5) city.population = std::stoi(curProv[4]);
    if (curProv.size() >= 6) city.wealth = std::stoi(curProv[5]);
    if (curProv.size() >= 7) city.food = std::stoi(curProv[6]);
    if (curProv.size() >= 8) city.production = std::stoi(curProv[7]);
    if (curProv.size() == 9) city.strength = std::stoi(curProv[8]);

    auto color = Province::Color(curProv[1]);
    usedColors.emplace(color); // Remember this color so we can check adjacency later
    queuedProvinces.push_back({ // Queue this province so we can render all of them at once
      curProv[0],
      color,
      curProv[2],
      city
    });
  }

  // Generate queued provinces
  for (const auto&[id, color, name, city] : queuedProvinces)
    provinces.emplace(id, Province(errorHandler, mapPath.c_str(), color, name, city, usedColors));

  // Generate adjacency map
  for (const auto& [name, prov] : provinces) {
    std::unordered_set<std::string> adjProvs;
    for (const auto& color : prov.getAdjacentColors()) {
      for (const auto& [otherName, otherProv] : provinces) {
        if (otherProv.getColor() != color) continue;
        adjProvs.emplace(otherName);
        break;
      }
    } adjacencyMap[name] = adjProvs;
  }
}

void ProvinceManager::render(const Window &window, const float scale, const vec2f &offset) {
  provShader.use();
  for (auto &province: provinces | std::views::values) province.render();

  if (scale > 0.5f) return; // Don't render text if zoomed in too far
  // TODO(Dory): Don't render text if it's offscreen

  // TODO(Dory): Find a better way to do province name text
  textShader.use();
  for (auto &[name, province] : provinces) {
    text.setText(name, 5.0f, province.getCenter(), static_cast<vec2f>(window.getDimensions()), offset);
    text.render();
  }
}

std::string ProvinceManager::clickedOnProvince(const float x, const float y) {
  for (auto &[name, province] : provinces) if (province.clickedOn(x, y)) return name;
  return "";
}
