#include "window.hpp"

Window::Window(const int width, const int height, const char* title, ErrorHandler *errorHandler) :
errorHandler(errorHandler) {
  if (!glfwInit()) errorHandler->logFatal("Failed to initialize GLFW", ErrorHandler::WINDOW_CREATION_ERROR);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // Make window not resizable

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!_window) {
    glfwTerminate();
    errorHandler->logFatal("Failed to create window", ErrorHandler::WINDOW_CREATION_ERROR);
  }

  glfwMakeContextCurrent(_window);
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    glfwTerminate();
    errorHandler->logFatal("Failed to initialize GLAD", ErrorHandler::WINDOW_CREATION_ERROR);
  }

  glViewport(0, 0, width, height);
  glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
}
