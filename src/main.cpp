#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils.hpp"
#include "window/window.hpp"
#include "province_manager/province_manager.hpp"
#include "city/city.hpp"

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
    offset.y += 0.001f * scale;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    offset.y -= 0.001f * scale;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    offset.x -= 0.001f * scale;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    offset.x += 0.001f * scale;
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

int main() {
  Window window(800, 600, "Caesar Engine");

  ProvinceManager pm;

  glfwSwapInterval(0); // Disable VSync
  
  glfwSetWindowUserPointer(window.window(), (void*)&pm);
  glfwSetMouseButtonCallback(window.window(), mouse_click_callback);
  glfwSetScrollCallback(window.window(), scroll_callback);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /*
  City city({"Test", pm.getProvince("PR1"), {0.0f, 0.0f}, 1000, 10, 100, 0});
  std::cout << city.getName() << std::endl;
  std::cout << city.getProvince().getName() << std::endl;
  std::cout << city.getPosition().x << ", " << city.getPosition().y << std::endl;
  std::cout << city.getPopulation() << std::endl;
  std::cout << city.getWealth() << std::endl;
  std::cout << city.getFood() << std::endl;
  std::cout << city.getIndustry() << std::endl;
  */

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
    pm.render(window);

    window.swapBuffers();
    window.pollEvents();
  }
  return EXIT_SUCCESS;
}
