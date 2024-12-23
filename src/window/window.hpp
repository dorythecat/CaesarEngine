#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

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
private:
  GLFWwindow* _window;

  static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
      glViewport(0, 0, width, height);
  }
};

#endif // WINDOW_HPP
