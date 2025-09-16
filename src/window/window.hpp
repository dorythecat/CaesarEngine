#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../utils.hpp"
#include "../error_handler/error_handler.h"

class Window {
public:
  Window(int width, int height, const char* title, ErrorHandler *errorHandler);
  ~Window() {
      glfwDestroyWindow(_window);
      glfwTerminate();
  }

  static void clear(const float r, const float g, const float b, const float a = 1.0f) {
      glClearColor(r, g, b, a);
      glClear(GL_COLOR_BUFFER_BIT);
  }
  static void clear(const float grey, const float a = 1.0f) { clear(grey, grey, grey, a); }

  void swapBuffers() const { glfwSwapBuffers(_window); }

  [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(_window) == GLFW_TRUE; }

  [[nodiscard]] GLFWwindow* window() const { return _window; }

  [[nodiscard]] vec2i getDimensions() const {
      vec2i dimensions;
      glfwGetWindowSize(_window, &dimensions.x, &dimensions.y);

      // This avoids crashes when minimizing the window
      if (dimensions.x <= 0) dimensions.x = 1;
      if (dimensions.y <= 0) dimensions.y = 1;

      return dimensions;
  }

private:
  GLFWwindow* _window;
  ErrorHandler* errorHandler;

  static void framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
      glViewport(0, 0, width, height);
  }
};

#endif // WINDOW_HPP
