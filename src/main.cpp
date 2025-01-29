#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>
#include <iostream>
#include <fstream>

#include "window/window.hpp"
#include "shader/shader.hpp"
#include "province/province.hpp"

std::map<std::string, Province> p;
float scale = 1.0f;
float offsetX = 0.0f;
float offsetY = 0.0f;

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
    offsetY += 0.01f;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    offsetY -= 0.01f;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    offsetX -= 0.01f;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    offsetX += 0.01f;
  }
}

void mouse_click_callback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    x = (x / 800.0) * 2.0 - 1.0;
    y = - (y / 600.0) * 2.0 + 1.0;
    for (auto& m : p) {
      if (m.second.clickedOn((float)x, (float)y)) {
        std::cout << "Clicked on province: " << m.first << std::endl;
        break;
      }
    }
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

int main() {
  Window window(800, 600, "Caesar Engine");

  Shader shader("res/shaders/default.vert", "res/shaders/default.frag");

  std::ifstream provinces("res/provinces.txt");
  if (!provinces.is_open()) {
    std::cerr << "Failed to open provinces.txt" << std::endl;
    return EXIT_FAILURE;
  }

  unsigned int i = 0; // Line number (mainly for debug purposes) 
  for (std::string line; std::getline(provinces, line);) {
    i++;
    // Skip empty lines, lines with only whitespaces, newlines, and comments
    if (line.length() == 0 ||
        line.find_first_not_of(' ') == std::string::npos ||
        line.substr(line.find_first_not_of(' '), 1) == "\n" ||
        line.substr(line.find_first_not_of(' '), 1) == "#") continue;
    line = line.substr(line.find_first_not_of(' '));

    std::istringstream lineStream(line);
    std::vector<std::string> curProv;
    for (std::string cur; std::getline(lineStream, cur, ',');) curProv.push_back(cur);

    if (curProv.size() < 3) {
      std::cerr << "ERROR: Province defined at line " << i << " is improperly formatted." << std::endl;
      continue;
    }
    p.emplace(curProv[0], Province("res/test.png", Province::Color(curProv[1]), curProv[2]));
  }

  glfwSwapInterval(0); // Disable VSync
  glfwSetMouseButtonCallback(window.window(), mouse_click_callback);
  glfwSetScrollCallback(window.window(), scroll_callback);

  double time, deltaTime, lastFrame = 0.0f;
  while(!window.shouldClose()) {
    time = glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;

    window.clear(0.0f, 0.0f, 0.0f, 1.0f);
    processInput(window.window());
    shader.setFloat("scale", scale);
    shader.setVec2("offset", offsetX, offsetY);
    shader.use();
    for (auto& m : p) {
      m.second.render();
    }

    window.swapBuffers();
    window.pollEvents();
  }

  p.clear(); // If we don't clear the map, a segmentation fault will occur on program exit

  return EXIT_SUCCESS;
}
