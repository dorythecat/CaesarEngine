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
#include "text/text.hpp"

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
    offsetY += 0.001f * scale;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    offsetY -= 0.001f * scale;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    offsetX -= 0.001f * scale;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    offsetX += 0.001f * scale;
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
    xf = xf - scale + offsetX;
    yf = scale - yf + offsetY;
    for (auto& m : p) {
      if (m.second.clickedOn(xf, yf)) {
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

  Text text("res/text.png", "res/text.csv");
  text.setText("Abcdefg\nhig", 100.0f, 100.0f, 50.0f);
  Shader textShader("res/shaders/text.vert", "res/shaders/text.frag");

  int width, height, nrChannels;
  unsigned char *data = stbi_load("res/heightmap.png", &width, &height, &nrChannels, 0);
  if (!data) {
    std::cerr << "Failed to load heightmap.png" << std::endl;
    return EXIT_FAILURE;
  }

  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
  glBindTexture(GL_TEXTURE_2D, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  double time, deltaTime, lastFrame = 0.0f;
  while(!window.shouldClose()) {
    time = glfwGetTime();
    deltaTime = time - lastFrame;
    lastFrame = time;

    window.clear(0.5f, 0.5f, 0.5f, 1.0f);
    processInput(window.window());
    shader.use();
    shader.setFloat("scale", scale);
    shader.setVec2f("offset", offsetX, offsetY);
    glBindTexture(GL_TEXTURE_2D, texture);
    shader.setInt("heightmap", 0);
    for (auto& m : p) {
      m.second.render();
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    textShader.use();
    textShader.setInt("tex", 0);
    text.render();

    window.swapBuffers();
    window.pollEvents();
  }

  p.clear(); // If we don't clear the map, a segmentation fault will occur on program exit

  return EXIT_SUCCESS;
}
