#include "province_manager.hpp"

#include <ranges>

ProvinceManager::ProvinceManager(const std::string &provShaderPath,
                                 const std::string &textShaderPath,
                                 const std::string& mapPath,
                                 const std::string& provPath) {
  std::ifstream province_file(provPath);
  if (!province_file.is_open()) {
    // TODO(Dory): Proper error handling
    std::cerr << "FATAL ERROR: Could not open file \"" << provPath << "\"" << std::endl;
    return;
  }

  unsigned int i = 0;
  for (std::string line; std::getline(province_file, line);) {
    i++;
    if (line.empty()) continue;
    line = line.substr(line.find_first_not_of(' '));
    std::string first = line.substr(0, 1);
    if (first == "#") continue;

    std::istringstream lineStream(line);
    std::vector<std::string> curProv;
    for (std::string cur; std::getline(lineStream, cur, ',');) {
      cur = cur.substr(cur.find_first_not_of(' '));
      curProv.push_back(cur);
    }

    if (curProv.size() < 4) {
      std::cerr << "ERROR: Province defined at line " << i << " lacks required information." << std::endl;
      continue;
    }
    provinces.emplace(curProv[0], Province(mapPath.c_str(),
      Province::Color(curProv[1]), curProv[2], std::stoi(curProv[3])));
  }

  provShader = Shader(provShaderPath);
  textShader = Shader(textShaderPath);
}

void ProvinceManager::render(const Window &window, const float scale, const vec2f &offset) {
  provShader.use();
  for (auto &province: provinces | std::views::values) province.render();

  if (scale > 0.5f) return; // Don't render text if zoomed in too far
  // TODO(Dory): Don't render text if it's offscreen

  // TODO(Dory): Find a better way to do province name text
  textShader.use();
  for (auto &[name, province] : provinces) {;
    text.setText(name, 5.0f, province.getCenter(), static_cast<vec2f>(window.getDimensions()), offset);
    text.render();
  }
}

std::string ProvinceManager::clickedOnProvince(const float x, const float y) {
  for (auto &[name, province] : provinces) if (province.clickedOn(x, y)) return name;
  return "";
}
