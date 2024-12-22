#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/window.hpp"

int main() {
  Window window(800, 600, "Caesar Engine");

  while(!window.shouldClose()) {
    window.clear(0.2f, 0.3f, 0.3f, 1.0f);

    window.swapBuffers();
    window.pollEvents();
  }

  return 0;
}
