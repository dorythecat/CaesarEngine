#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "../utils.hpp"

class Window {
public:
  Window(int width, int height, const char* title);
  ~Window() {
      glfwDestroyWindow(_window);
      glfwTerminate();
  }

  void clear(float r, float g, float b, float a) {
      glClearColor(r, g, b, a);
      glClear(GL_COLOR_BUFFER_BIT);
  }

  void swapBuffers() {
      glfwSwapBuffers(_window);
  }

  void pollEvents() {
      glfwPollEvents();
  }

  bool shouldClose() {
      return glfwWindowShouldClose(_window);
  }

  GLFWwindow* window() {
      return _window;
  }

  vec2i getDimensions() {
      vec2i dimensions;
      glfwGetWindowSize(_window, &dimensions.x, &dimensions.y);
      return dimensions;
  }

private:
  GLFWwindow* _window;

  static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
      glViewport(0, 0, width, height);
  }
};

#endif // WINDOW_HPP
