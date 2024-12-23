#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "window/window.hpp"
#include "shader/shader.hpp"
#include "mesh/mesh.hpp"

std::vector<Mesh::Vertex> vertices = {
  Mesh::Vertex(0.5f,  0.5f),
  Mesh::Vertex(0.5f, -0.5f),
  Mesh::Vertex(-0.5f, -0.5f),
  Mesh::Vertex(-0.5f,  0.5f) 
};
std::vector<unsigned int> indices = {
  0, 1, 3,
  1, 2, 3
};

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
  Mesh mesh = Mesh(vertices, indices, shader);

  while(!window.shouldClose()) {
    window.clear(0.2f, 0.3f, 0.3f, 1.0f);

    processInput(window.window());

    mesh.render();

    window.swapBuffers();
    window.pollEvents();
  }

  return 0;
}
