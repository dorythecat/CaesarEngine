#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "window/window.hpp"
#include "shader/shader.hpp"


float vertices[] = {
   0.5f,  0.5f, 0.0f,  // top right
   0.5f, -0.5f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  // bottom left
  -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
  0, 1, 3,   // first triangle
  1, 2, 3    // second triangle
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

  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glBindVertexArray(0);

  Shader shader("res/shaders/default.vert", "res/shaders/default.frag");

  while(!window.shouldClose()) {
    window.clear(0.2f, 0.3f, 0.3f, 1.0f);

    processInput(window.window());

    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    window.swapBuffers();
    window.pollEvents();
  }

  return 0;
}
