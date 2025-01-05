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

int main() {
  Window window(800, 600, "Caesar Engine");

  Shader shader("res/shaders/default.vert", "res/shaders/default.frag");

  std::ifstream provinces("res/provinces.txt");
  if (!provinces.is_open()) {
    std::cerr << "Failed to open provinces.txt" << std::endl;
    return EXIT_FAILURE;
  }

  std::string line;
  unsigned int i = 0; // Line number (mainly for debug purposes)
  std::string currentProvince;
  
  // We save all this data to generate the mesh AFTER reading all the data, so that we don't nmeed to care about the order it's presented in.
  std::string color;
  std::string name;
  while (std::getline(provinces, line)) {
    i++;
    // Skip empty lines, lines with only whitespaces, newlines, and comments
    if (line.length() == 0 ||
        line.find_first_not_of(' ') == std::string::npos ||
        line.substr(line.find_first_not_of(' '), 1) == "\n" ||
        line.substr(line.find_first_not_of(' '), 1) == "#") continue;
    if (line[0] != ' ' && line[0] != '\t') { // New province, save ID
      if (color.empty() && !currentProvince.empty()) {
        std::cerr << "ERROR: Province \"" << currentProvince << "\" lacks the \"color\" attribute, which is mandatory! A mesh will not be generated for said province!" << std::endl;
      }

      // Generate the province
      if (!currentProvince.empty()) { // Skip the first iteration
        p.emplace(currentProvince, Province("res/test.png", Province::Color(color), name));
      }

      // Reset variables
      color.clear();
      name.clear();

      // Save the new province ID
      currentProvince = line.substr(0, line.find_first_of(':'));
      continue;
    }
    line = line.substr(line.find_first_not_of(' '));
    if (line.contains("color:")) {
      if (!color.empty()) {
        std::cerr << "ERROR ON LINE " << i - 1 << ":  \"color\" value already defined for province: " << currentProvince << std::endl;
        continue;
      }
      color = line.substr(6);
      if (color.length() < 6) { // If it's below 6 characters, it's not a valid hex color code
        std::cerr << "ERROR ON LINE " << i - 1 << ": Invalid color code: \"" << color << "\"" << std::endl;
        continue;
      }
      if (color.length() > 6) { // Trim any leading whitespace
        color = color.substr(line.find_first_not_of(' ') + 1, 6);
      }
      continue;
    }
    if (line.contains("name:")) {
      name = line.substr(5);
      if (name.length() < 1) {
        std::cerr << "ERROR ON LINE " << i - 1 << ": Province name too short!" << std::endl;
        continue;
      }
      if (name.length() > 32) {
        std::cerr << "ERROR ON LINE " << i - 1 << ": Province name too long!" << std::endl;
        continue;
      }
    }
  }

  // This is to catch the last province, since it won't be caught by the loop
  if (color.empty()) {
    std::cerr << "ERROR: Province \"" << currentProvince << "\" lacks the \"color\" attribute, which is mandatory! A mesh will not be generated for said province!" << std::endl;
  } else {
    p.emplace(currentProvince, Province("res/test.png", Province::Color(color), name));
  }

  glfwSwapInterval(0); // Disable VSync
  glfwSetMouseButtonCallback(window.window(), mouse_click_callback);

  double time, deltaTime, lastFrame = 0.0f;
  while(!window.shouldClose()) {
    time = glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;

    window.clear(0.0f, 0.0f, 0.0f, 1.0f);
    processInput(window.window());
    shader.use();
    for (auto& m : p) {
      m.second.render();
    }

    window.swapBuffers();
    window.pollEvents();
  }

  return EXIT_SUCCESS;
}
