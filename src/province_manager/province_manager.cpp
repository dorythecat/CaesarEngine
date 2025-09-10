#include "province_manager.hpp"

#include "../province/province.hpp"

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
  } province_file.close();

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

void ProvinceManager::render(const Window& window, const float scale, const vec2f& offset) {
  provShader.use();
  for (auto &province: provinces | std::views::values) province.render();

  lineShader.use();
  line.render();

  if (scale > 0.5f) return; // Don't render text if zoomed out too far
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

ProvinceManager::Connection ProvinceManager::findPath(const std::string& provinceA, const std::string& provinceB) {
  Connection connection;
  if (!provinces.contains(provinceA) || !provinces.contains(provinceB)) return connection;
  if (provinceA == provinceB) {
    connection.steps = 0;
    return connection;
  }

  for (const auto& conn : connectionCache) {
    if (conn.provinces.front().first == provinceA && conn.provinces.back().first == provinceB) {
      // Move to front of cache
      std::ranges::remove_if(connectionCache, [&conn](const Connection &c) { return c == conn; });
      connectionCache.push_front(conn);

      std::vector<vec2f> linePoints;
      for (const auto &prov: conn.provinces | std::views::values) linePoints.push_back(prov.getCenter());
      line.setPoints(linePoints);

      return conn; // Return cached connection
    }

    if (conn.provinces.front().first == provinceB && conn.provinces.back().first == provinceA) {
      Connection reversedConn = conn;
      std::ranges::reverse(reversedConn.provinces);
      connectionCache.push_front(reversedConn); // Keep the other one in its current position

      std::vector<vec2f> linePoints;
      for (const auto &prov: reversedConn.provinces | std::views::values) linePoints.push_back(prov.getCenter());
      line.setPoints(linePoints);

      return reversedConn; // Return reversed cached connection
    }
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
    std::unordered_set<std::string> adjSet = adjacencyMap.at(cur);
    std::vector<std::string> adjProvs(adjSet.begin(), adjSet.end());
    std::ranges::sort(adjProvs, [&provinces = this->provinces](const std::string &a, const std::string &b) {
      return provinces.at(a).getArea() < provinces.at(b).getArea();
    });

    for (const auto& adj : adjProvs) {
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
  std::vector<vec2f> linePoints;
  for (std::string cur = provinceB; cur != provinceA;) {
    Province prov = provinces.at(cur);
    path.emplace_front(cur, prov);
    linePoints.push_back(prov.getCenter());
    auto it = std::ranges::find_if(parent,
      [&cur](const std::pair<std::string, std::string> &p) { return p.first == cur; });
    if (it == parent.end()) break; // Shouldn't happen
    cur = it->second;
  } path.emplace_front(provinceA, provinces.at(provinceA));
  line.setPoints(linePoints); // Generate line points

  // Generate connection
  connection.steps = static_cast<int>(path.size()) - 1;
  connection.provinces = path;
  connectionCache.push_front(connection);
  if (connectionCache.size() > MAX_PATH_SAVE) connectionCache.pop_back(); // Limit cache size

  return connection;
}
