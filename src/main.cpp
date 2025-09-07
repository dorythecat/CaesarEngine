#define STB_IMAGE_IMPLEMENTATION
#define STB_ONLY_PNG

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <unordered_map>

#include "utils.hpp"
#include "window/window.hpp"
#include "state_manager/state_manager.hpp"
#include "error_handler/error_handler.h"

enum KEYBINDS_ENUM {
    EXIT = 0,
    DEBUG_WIREFRAME_ON = 1,
    DEBUG_WIREFRAME_OFF = 2,
    MOVE_UP = 3,
    MOVE_DOWN = 4,
    MOVE_LEFT = 5,
    MOVE_RIGHT = 6,
    CLICK_KEY = 7,
    DRAG_KEY = 8
};

std::unordered_map<KEYBINDS_ENUM, int> keybinds = {
    {EXIT, GLFW_KEY_ESCAPE},
    {DEBUG_WIREFRAME_ON, GLFW_KEY_F5},
    {DEBUG_WIREFRAME_OFF, GLFW_KEY_F6},
    {MOVE_UP, GLFW_KEY_W},
    {MOVE_DOWN, GLFW_KEY_S},
    {MOVE_LEFT, GLFW_KEY_A},
    {MOVE_RIGHT, GLFW_KEY_D},
    {CLICK_KEY, GLFW_MOUSE_BUTTON_LEFT},
    {DRAG_KEY, GLFW_MOUSE_BUTTON_RIGHT},
};

float scale = 1.0f;
vec2f offset;

// Keybind utilities
bool keyPressed(GLFWwindow* window, const KEYBINDS_ENUM key) {
    return glfwGetKey(window, keybinds[key]) == GLFW_PRESS;
}

void processInput(GLFWwindow* window) {
    // Exit on ESC
    if (keyPressed(window, EXIT)) glfwSetWindowShouldClose(window, true);

    // Debug wireframe mode (on with F5, off with F6)
    if (keyPressed(window, DEBUG_WIREFRAME_ON))
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (keyPressed(window, DEBUG_WIREFRAME_OFF))
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Move the view with WASD keys
    if (keyPressed(window, MOVE_UP)) offset.y += scale * 0.001f;
    if (keyPressed(window, MOVE_DOWN)) offset.y -= scale * 0.001f;
    if (keyPressed(window, MOVE_LEFT)) offset.x -= scale * 0.001f;
    if (keyPressed(window, MOVE_RIGHT)) offset.x += scale * 0.001f;
}

void mouse_click_callback(GLFWwindow* window,
                          const int button,
                          const int action,
                          int mods) {
    if (button != keybinds[CLICK_KEY] || action != GLFW_PRESS) return;
    vec2d position;
    glfwGetCursorPos(window, &position.x, &position.y);
    vec2i dimensions;
    glfwGetWindowSize(window, &dimensions.x, &dimensions.y);
    // At some point we could change the system to work with double-precision floats,
    // which would be more accurate but use more memory
    vec2f f = static_cast<vec2f>(position) * 2.0f * scale / static_cast<vec2f>(dimensions);
    f = (f - scale) * vec2f(1.0f, -1.0f) + offset;
    const auto *sm = static_cast<StateManager *>(glfwGetWindowUserPointer(window));
    if (const std::string state = sm->clickedOnState(f); !state.empty()) {
        const std::string provinceName = sm->pm->clickedOnProvince(f);
        const Province p = sm->pm->getProvince(provinceName);
        std::cout << "Clicked on province: " << p.getName() << ", on state: " << state << std::endl;

        for (auto adjacencyMap = sm->pm->getAdjacencyMap();
             const auto& adjProvName : adjacencyMap[provinceName]) {
            std::cout << " - Adjacent province: " << sm->pm->getProvince(adjProvName).getName() << std::endl;
        }
    }
}

void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
    if (yoffset < 0.0) {
        if (scale > 10.0f) return;
        scale += 0.1f;
        return;
    }
    if (scale < 0.1f) return;
    scale -= 0.1f;
}

vec2f lastMousePos;
void mouse_cursor_callback(GLFWwindow* window, const double xpos, const double ypos) {
    const auto mousePos = static_cast<vec2f>(vec2d(xpos, ypos));
    if (glfwGetMouseButton(window, keybinds[DRAG_KEY]) == GLFW_RELEASE) {
        lastMousePos = mousePos;
        return;
    }

    const vec2f delta = mousePos - lastMousePos;
    if (delta == vec2f()) return; // No movement
    // Remember 1: We need to subtract the delta because the movement is inverted
    // Remember 2: The y-axis is inverted in window coordinates
    offset -= delta * scale * vec2f(0.002f, -0.002f);
    lastMousePos = mousePos;
}

int main() {
    ErrorHandler errorHandler;

    const Window window(800, 600, "Caesar Engine", &errorHandler);

    StateManager sm(&errorHandler);

    glfwSwapInterval(0); // Disable VSync

    glfwSetWindowUserPointer(window.window(), &sm);
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
        sm.pm->provShader.setVec2f("offset", offset);

        sm.pm->textShader.use();
        sm.pm->textShader.setFloat("scale", scale);
        sm.pm->textShader.setVec2f("offset", offset);
        sm.pm->textShader.setInt("tex", 0);

        // Render states (and therefore provinces)
        sm.render(window, scale, offset);

        window.swapBuffers();
        glfwPollEvents();
    } return EXIT_SUCCESS;
}
