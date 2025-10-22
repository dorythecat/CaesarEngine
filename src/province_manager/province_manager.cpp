#include "province_manager.hpp"

ProvinceManager::ProvinceManager(ErrorHandler* errorHandler,
                                 const std::string& provShaderPath,
                                 const std::string& textShaderPath,
                                 const std::string& lineShaderPath,
                                 const std::string& mapPath,
                                 const std::string& provPath) : provShader(errorHandler, provShaderPath),
                                                                textShader(errorHandler, textShaderPath),
                                                                lineShader(errorHandler, lineShaderPath),
                                                                text(errorHandler),
                                                                errorHandler(errorHandler),
                                                                line(errorHandler) {
  std::ifstream province_file(provPath);
  if (!province_file.is_open()) errorHandler->logFatal("Could not open file \"" + provPath + "\"",
    ErrorHandler::COULD_NOT_OPEN_FILE_ERROR);

  size_t i = 0; // Line number
  std::vector<QueuedProvince> queuedProvinces; // Read the provinces, queue them, and then generate them
  std::unordered_set<Province::Color, Province::Color::HashFunction> usedColors; // For adjacency optimizations
  for (std::string fileLine; std::getline(province_file, fileLine); i++) {
    if (fileLine.empty()) continue;
    fileLine = fileLine.substr(fileLine.find_first_not_of(' '));
    if (fileLine.substr(0, 1) == "#") continue; // Check for comments

    std::istringstream fileLineStream(fileLine);
    std::array<std::string, 9> curProv;
    size_t elements = 0;
    for (std::string cur; std::getline(fileLineStream, cur, ','); elements++) {
      if (elements >= 9) {
        errorHandler->logWarning("Province defined at line " + std::to_string(i) + " has too many parameters.",
          ErrorHandler::FORMAT_ERROR);
        break;
      } curProv[elements] = cur.substr(cur.find_first_not_of(' '));
    }

    if (elements < 4) {
      errorHandler->logFatal("Province defined at line " + std::to_string(i) + " lacks required information.",
        ErrorHandler::FORMAT_ERROR);
      break;
    }

    auto city = Province::City(errorHandler, static_cast<Province::City::CityCategory>(std::stoi(curProv[3])));
    switch (elements) {
      case 9: city.strength = std::stoi(curProv[8]); [[fallthrough]];
      case 8: city.production = std::stoi(curProv[7]); [[fallthrough]];
      case 7: city.food = std::stoi(curProv[6]); [[fallthrough]];
      case 6: city.wealth = std::stoi(curProv[5]); [[fallthrough]];
      case 5: city.population = std::stoi(curProv[4]); [[fallthrough]];
      default: break;
    }

    auto color = Province::Color(curProv[1]);
    // Remember this color so we can check adjacency later, but only if it's not a wasteland
    if (city.category != Province::City::WASTELAND) usedColors.emplace(color);
    queuedProvinces.push_back({ // Queue this province so we can render all of them at once
      curProv[0],
      color,
      curProv[2],
      city
    });
  } province_file.close();

  // Generate queued provinces
  for (const auto&[id, color, name, city] : queuedProvinces)
    provinces.emplace(id, Province(errorHandler, mapPath.c_str(), color, name, city, usedColors));

  // Generate adjacency map
  for (const auto& [name, prov] : provinces) {
    if (prov.city.category == Province::City::WASTELAND) continue; // Wastelands aren't connected to anything
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

void ProvinceManager::render(const Window& window,
                             const float scale,
                             const vec2f& offset,
                             const std::unordered_map<std::string, Province::Color>& provColors) {
  provShader.use();
  for (auto &[id, province]: provinces) {
    const auto color = provColors.at(id);
    provShader.setVec3f("color",
      static_cast<float>(color.r) / 255.0f,
      static_cast<float>(color.g) / 255.0f,
      static_cast<float>(color.b) / 255.0f);
    provShader.setVec2f("center", province.getCenter());
    province.render();
  }

  lineShader.use();
  line.render();

  // Don't render text if zoomed out too far or offscreen
  if (scale > 0.5f || offset > vec2f(1.0f) || offset < vec2f(-1.0f)) return;

  // TODO(Dory): Find a better way to do province name text
  textShader.use();
  for (auto &[name, province] : provinces) {
    text.setText(name, 5.0f, province.getCenter(), static_cast<vec2f>(window.getDimensions()), offset);
    textShader.setVec2f("center", province.getCenter());
    text.render();
  }
}

std::string ProvinceManager::clickedOnProvince(const float x, const float y) {
  for (auto &[name, province] : provinces) if (province.clickedOn(x, y)) return name;
  return "";
}

ProvinceManager::Connection ProvinceManager::findPath(const std::string& provinceA, const std::string& provinceB) {
  Connection connection;
  if (!provinces.contains(provinceA) || !provinces.contains(provinceB) ||
      provinces.at(provinceA).city.category == Province::City::WASTELAND ||
      provinces.at(provinceB).city.category == Province::City::WASTELAND) return connection;
  if (provinceA == provinceB) {
    connection.steps = 0;
    return connection;
  }

  // BFS to find the shortest path
  std::list<std::pair<std::string, std::string>> parent;
  std::queue<std::string> toVisit;
  std::unordered_set<std::string> visited;

  toVisit.push(provinceA); // Start from province A

  bool found = false;
  while (!toVisit.empty()) {
    // Get first element
    std::string cur = toVisit.front();
    toVisit.pop();

    // Sort for optimization
    std::vector adjProvs(adjacencyMap.at(cur).begin(), adjacencyMap.at(cur).end());
    std::ranges::sort(adjProvs, [&provinces = this->provinces](const std::string& a, const std::string& b) {
      return provinces.at(a).getArea() < provinces.at(b).getArea();
    });

    for (const std::string& adj : adjProvs) {
      if (visited.contains(adj)) continue;
      parent.emplace_back(adj, cur);
      toVisit.push(adj);
      if (adj != provinceB) continue;
      found = true;
      break;
    } visited.emplace(cur);
    if (found) break;
  } if (!found) return connection; // Not connected

  // Reconstruct path
  std::list<std::pair<std::string, Province>> path;
  for (std::string cur = provinceB; cur != provinceA;) {
    path.emplace_front(cur, provinces.at(cur));
    auto it = std::ranges::find_if(parent,
      [&cur](const std::pair<std::string, std::string> &p) { return p.first == cur; });
    if (it == parent.end()) break; // Shouldn't happen
    cur = it->second;
  } path.emplace_front(provinceA, provinces.at(provinceA));

  // Generate connection
  connection.steps = static_cast<int>(path.size()) - 1;
  connection.provinces = path;

  std::vector<vec2f> linePoints;
  for (const auto &prov: path | std::views::values) linePoints.push_back(prov.getCenter());
  line.setPoints(linePoints);

  connection.length = line.length;

  return connection;
}