#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

#include "utils.hpp"
#include "window/window.hpp"
#include "province_manager/province_manager.hpp"
#include "state/state.hpp"
#include "text/text.hpp"

float scale = 1.0f;
vec2f offset;

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }

  if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    offset.y += 0.001f;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    offset.y -= 0.001f;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    offset.x -= 0.001f;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    offset.x += 0.001f;
  }
}

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // At some point we could change the system to work with double-precision floats, which would be more accurate, but use more memory
    float scaleFact = 2.0f * scale;
    float xf = scaleFact * static_cast<float>(x) / static_cast<float>(width);
    float yf = scaleFact * static_cast<float>(y) / static_cast<float>(height);
    xf = xf - scale + offset.x;
    yf = scale - yf + offset.y;
    ProvinceManager *pm = static_cast<ProvinceManager*>(glfwGetWindowUserPointer(window));
    std::string province = pm->clickedOnProvince(xf, yf);
    if (province != "") std::cout << "Clicked on province: " << province << std::endl;
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (yoffset < 0.0) {
    if (scale > 10.0f) return;
    scale += 0.1f;
    return;
  }
  if (scale < 0.1f) return;
  scale -= 0.1f;
}

double lastX = 0.0;
double lastY = 0.0;
void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
    lastX = xpos;
    lastY = ypos;
    return;
  }

  double deltaX = xpos - lastX;
  double deltaY = ypos - lastY;
  if (deltaX == 0.0 && deltaY == 0.0) {
    return; // No movement
  }
  offset.x -= deltaX * scale * 0.002f; // Scale the offset by the scale factor
  offset.y += deltaY * scale * 0.002f; // Invert the y-axis to match the OpenGL coordinate system
  lastX = xpos;
  lastY = ypos;
}

int main() {
  Window window(800, 600, "Caesar Engine");

  ProvinceManager pm;

  glfwSwapInterval(0); // Disable VSync
  
  glfwSetWindowUserPointer(window.window(), (void*)&pm);
  glfwSetMouseButtonCallback(window.window(), mouse_click_callback);
  glfwSetScrollCallback(window.window(), scroll_callback);
  glfwSetCursorPosCallback(window.window(), mouse_cursor_callback);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Load states
  // TODO(Dory): Make this its own class or move into somewhere else
  std::ifstream stateFile("res/states.txt");
  if (!stateFile.is_open()) {
    // TODO(Dory): Proper error handling
    std::cerr << "FATAL ERROR: Could not open file \"res/states.txt\"" << std::endl;
    return EXIT_FAILURE;
  }

  std::map<std::string, State> states;
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
        for (std::string provinceId; std::getline(curStream, provinceId, ',');) {
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
      return EXIT_FAILURE;
    }

    if (provinceIds.empty()) {
      std::cerr << "FATAL ERROR: State " << id << " has no provinces" << std::endl;
      return EXIT_FAILURE;
    }

    State state(name);
    for (const auto &provinceId : provinceIds) state.addProvince(pm.getProvince(provinceId));
    states.emplace(id, state);
  }

  double time, deltaTime, lastFrame = 0.0f;
  while(!window.shouldClose()) {
    time = glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;

    window.clear(0.5f, 0.5f, 0.5f, 1.0f);
    processInput(window.window());

    // Setup shaders
    pm.provShader.use();
    pm.provShader.setFloat("scale", scale);
    pm.provShader.setVec2f("offset", offset.x, offset.y);
    
    pm.textShader.use();
    pm.textShader.setFloat("scale", scale);
    pm.textShader.setVec2f("offset", offset.x, offset.y);
    pm.textShader.setInt("tex", 0);

    // Render provinces
    pm.render(window, scale, offset);

    pm.textShader.use();
    Text text;
    for (auto &state : states) {
      vec2i dimensions = window.getDimensions();
      text.setText(state.first,
                   (state.second.getCenterX() + 0.5f) * static_cast<float>(dimensions.x) - 10.0f,
                   (state.second.getCenterY() + 0.5f) * static_cast<float>(dimensions.y),
                   10.0f,
                   dimensions);
      text.render();
    }

    window.swapBuffers();
    window.pollEvents();
  }
  return EXIT_SUCCESS;
}
