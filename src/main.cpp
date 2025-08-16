#include <algorithm>
#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "utils.hpp"
#include "window/window.hpp"
#include "state_manager/state_manager.hpp"

float scale = 1.0f;
vec2f offset;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) offset.y += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) offset.y -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) offset.x -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) offset.x += 0.001f;
}

void mouse_click_callback(GLFWwindow *window,
                          const int button,
                          const int action,
                          int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        // At some point we could change the system to work with double-precision floats,
        // which would be more accurate but use more memory
        const float scaleFact = 2.0f * scale;
        float xf = scaleFact * static_cast<float>(x) / static_cast<float>(width);
        float yf = scaleFact * static_cast<float>(y) / static_cast<float>(height);
        xf = xf - scale + offset.x;
        yf = scale - yf + offset.y;
        auto *sm = static_cast<StateManager *>(glfwGetWindowUserPointer(window));
        if (const std::string state = sm->clickedOnState(xf, yf); !state.empty()) {
            const std::string provinceName = sm->pm->clickedOnProvince(xf, yf);
            const Province p = sm->pm->getProvince(provinceName);
            std::cout << "Clicked on province: " << p.getName() << ", on state: " << state << std::endl;
        }
    }
}

void scroll_callback(GLFWwindow *window, const double xoffset, const double yoffset) {
    if (yoffset < 0.0) {
        if (scale > 10.0f) return;
        scale += 0.1f;
        return;
    }
    if (scale < 0.1f) return;
    scale -= 0.1f;
}

float lastX = 0.0;
float lastY = 0.0;

void mouse_cursor_callback(GLFWwindow *window, const double xpos, const double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        return;
    }

    const float deltaX = static_cast<float>(xpos) - lastX;
    const float deltaY = static_cast<float>(ypos) - lastY;
    if (deltaX == 0.0f && deltaY == 0.0f) return; // No movement
    offset.x -= deltaX * scale * 0.002f; // Scale the offset by the scale factor
    offset.y += deltaY * scale * 0.002f; // Invert the y-axis to match the OpenGL coordinate system
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);
}

int main() {
    Window window(800, 600, "Caesar Engine");

    StateManager sm;

    glfwSwapInterval(0); // Disable VSync

    glfwSetWindowUserPointer(window.window(), (void *) &sm);
    glfwSetMouseButtonCallback(window.window(), mouse_click_callback);
    glfwSetScrollCallback(window.window(), scroll_callback);
    glfwSetCursorPosCallback(window.window(), mouse_cursor_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //double lastFrame = 0.0;
    while (!window.shouldClose()) {
        //const double time = glfwGetTime();
        //double deltaTime = time - lastFrame;
        //lastFrame = time;

        Window::clear(0.5f, 0.5f, 0.5f, 1.0f);
        processInput(window.window());

        // Setup shaders
        sm.pm->provShader.use();
        sm.pm->provShader.setFloat("scale", scale);
        sm.pm->provShader.setVec2f("offset", offset.x, offset.y);

        sm.pm->textShader.use();
        sm.pm->textShader.setFloat("scale", scale);
        sm.pm->textShader.setVec2f("offset", offset.x, offset.y);
        sm.pm->textShader.setInt("tex", 0);

        // Render states (and therefore provinces)
        sm.render(window, scale, offset);

        window.swapBuffers();
        glfwPollEvents();
    } return EXIT_SUCCESS;
}
