#include "province_manager.hpp"

ProvinceManager::ProvinceManager(std::string provShaderPath,
                                 std::string textShaderPath,
                                 std::string mapPath,
                                 std::string provPath) {
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
    if (first == "\n" || first == "#") continue;

    std::istringstream lineStream(line);
    std::vector<std::string> curProv;
    for (std::string cur; std::getline(lineStream, cur, ',');)
      curProv.push_back(cur);

    if (curProv.size() < 4) {
      std::cerr << "ERROR: Province defined at line " << i << " lacks required information." << std::endl;
      continue;
    }
    provinces.emplace(curProv[0], Province(mapPath.c_str(), Province::Color(curProv[1]), curProv[2], std::stoi(curProv[3])));
  }

  provShader = Shader(provShaderPath);
  textShader = Shader(textShaderPath);
}

void ProvinceManager::render(Window &window) {
  provShader.use();
  for (auto &province : provinces) {
    province.second.render();
  }

  textShader.use();
  for (auto &province : provinces) {
    vec2i dimensions = window.getDimensions();
    text.setText(province.first,
                 (province.second.getCenterX() + 0.5f) * static_cast<float>(dimensions.x) - 30.0f,
                 (province.second.getCenterY() + 0.5f) * static_cast<float>(dimensions.y),
                 30.0f,
                 dimensions);
    text.render();
  }
}

std::string ProvinceManager::clickedOnProvince(float x, float y) {
  for (auto &province : provinces) {
    if (province.second.clickedOn(x, y)) {
      return province.first;
    }
  } return "";
}

Province& ProvinceManager::getProvince(std::string name) {
  return provinces.at(name);
}
