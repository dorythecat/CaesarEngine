#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "window/window.hpp"
#include "shader/shader.hpp"
#include "mesh/mesh.hpp"

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

  Mesh image("res/test.png", shader);

  glfwSwapInterval(0);

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
    image.render();

    window.swapBuffers();
    window.pollEvents();
  }

  return 0;
}
