#include "window.hpp"

Window::Window(const int width, const int height, const char* title) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Make window not resizable

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!_window) {
    std::cerr << "Failed to create window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(_window);
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
}
