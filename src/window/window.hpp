#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../utils.hpp"

class Window {
public:
  Window(int width, int height, const char* title);
  ~Window() {
      glfwDestroyWindow(_window);
      glfwTerminate();
  }

  static void clear(const float r, const float g, const float b, const float a) {
      glClearColor(r, g, b, a);
      glClear(GL_COLOR_BUFFER_BIT);
  }

  void swapBuffers() const { glfwSwapBuffers(_window); }

  [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window) == GLFW_TRUE; }

  [[nodiscard]] GLFWwindow* window() const { return _window; }

  [[nodiscard]] vec2i getDimensions() const {
      vec2i dimensions;
      glfwGetWindowSize(_window, &dimensions.x, &dimensions.y);
      return dimensions;
  }

private:
  GLFWwindow* _window;

  static void framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
      glViewport(0, 0, width, height);
  }
};

#endif // WINDOW_HPP
