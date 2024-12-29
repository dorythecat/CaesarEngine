#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "window/window.hpp"
#include "shader/shader.hpp"
#include "mesh/mesh.hpp"

#define STB_IMAGE_IMPLEMENTATION
// Disable a few image formats we don't require (we just want PNG, at least for now)
#define STBI_ONLY_PNG

#include <stb_image.h>

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

  // Just some test code for now
  int x,y,n;
  unsigned char* data = stbi_load("res/test.png", &x, &y, &n, 0);
  if (!data) {
    std::cerr << "Failed to load image!" << std::endl;
    return EXIT_FAILURE;
  }
  std::vector<Mesh::Vertex> image_data(x * y * 4);
  std::vector<unsigned int> image_indices(x * y * 6);
  unsigned int j = 0;
  std::cout << "Loaded image with dimensions: " << x << "x" << y << std::endl;
  std::cout << "Number of channels: " << n << std::endl;
  for (int i = 0; i < x * y * n; i+=n) {
    if (data[i] != 0) continue;
    int m = i / n;
    float p = (float)(m % x) / (float)x - 1.0f;
    float q = - (float)(m / x) / (float)y;
    image_data[j] = Mesh::Vertex(
      p,
      q
    );
    image_data[j + 1] = Mesh::Vertex(
      p,
      q - 1 / (float)y
    );
    image_data[j + 2] = Mesh::Vertex(
      p + 1 / (float)x,
      q - 1 / (float)y
    );
    image_data[j + 3] = Mesh::Vertex(
      p + 1 / (float)x,
      q
    );

    // Generate indices for the quad
    image_indices.push_back(j);
    image_indices.push_back(j + 1);
    image_indices.push_back(j + 3);

    image_indices.push_back(j + 1);
    image_indices.push_back(j + 2);
    image_indices.push_back(j + 3);

    j += 4;
  } stbi_image_free(data);

  image_data.resize(j);
  image_indices.resize(j * 6);

  std::cout << "Generated " << j << " vertices and " << j * 6 << " indices" << std::endl;

  Mesh image(image_data, image_indices, shader);

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
