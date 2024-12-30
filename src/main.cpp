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

int main() {
  Window window(800, 600, "Caesar Engine");

  Shader shader("res/shaders/default.vert", "res/shaders/default.frag");

  std::ifstream provinces("res/provinces.txt");
  if (!provinces.is_open()) {
    std::cerr << "Failed to open provinces.txt" << std::endl;
    return EXIT_FAILURE;
  }

  std::map<std::string, Province> p;
  std::string line;
  unsigned int i = 0; // Line number (mainly for debug purposes)
  std::string currentProvince = "";
  bool meshGenerated = true; // Needs to start at true, or it will throw a senseless error
  while (std::getline(provinces, line)) {
    i++;
    // Skip empty lines, lines with only whitespaces, newlines, and comments
    if (line.length() == 0 ||
        line.find_first_not_of(' ') == std::string::npos ||
        line.substr(line.find_first_not_of(' '), 1) == "\n" ||
        line.substr(line.find_first_not_of(' '), 1) == "#") continue;
    if (line[0] != ' ' && line[0] != '\t') { // New province, save ID
      if (!meshGenerated) {
        std::cerr << "ERROR: Province \"" << currentProvince << "\" lacks the \"color\" attribute, which is mandatory! A mesh will not be generated for said province!" << std::endl;
      }
      currentProvince = line.substr(0, line.find_first_of(':'));
      meshGenerated = false;
      continue;
    }
    line = line.substr(line.find_first_not_of(' '));
    if (!line.contains("color:")) continue;
    if (meshGenerated) {
      std::cerr << "ERROR ON LINE " << i-1 << ": Mesh already generated for province: " << currentProvince << std::endl;
      continue;
    }
    meshGenerated = true;
    std::string color = line.substr(6);
    if (color.length() < 6) { // If it's below 6 characters, it's not a valid hex color code
      std::cerr << "Invalid color: " << color << std::endl;
      continue;
    }
    if (color.length() > 6) { // Trim any leading whitespace
      color = color.substr(color.find_first_not_of(' '), 6);
    }
    p.emplace(currentProvince, Province("res/test.png", shader, Province::Color(color)));
  }

  // This is to catch the last province, since it won't be caught by the loop
  if (!meshGenerated) {
    std::cerr << "ERROR: Province \"" << currentProvince << "\" lacks the \"color\" attribute, which is mandatory! A mesh will not be generated for said province!" << std::endl;
   }

  glfwSwapInterval(0); // Disable VSync

  double time, deltaTime, lastFrame = 0.0f;
  unsigned int fps = 0, counter = 0;
  while(!window.shouldClose()) {
    time = glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;

    fps += (unsigned int)(1 / deltaTime);
    counter++;
    if (counter == 600) {
      std::cout << "FPS: " << fps / 600 << std::endl;
      fps = 0;
      counter = 0;
    }

    window.clear(0.0f, 0.0f, 0.0f, 1.0f);
    processInput(window.window());
    for (auto& m : p) {
      m.second.render();
    }

    window.swapBuffers();
    window.pollEvents();
  }

  return EXIT_SUCCESS;
}
