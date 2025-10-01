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
#include "ticker/ticker.h"

enum KEYBINDS_ENUM {
#ifdef DEBUG
    DEBUG_WIREFRAME_ON,
    DEBUG_WIREFRAME_OFF,
    DEBUG_TICK,
#endif
    EXIT,
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
};

enum MOUSE_KEYBINDS_ENUM {
    CLICK_KEY,
    DRAG_KEY
};

// Keybinds
// Each action can have multiple keybinds, and each keybind can contain multiple keys
// A keybind will only trigger if all the keys in it are pressed at the same time
// An action will trigger if any of its keybinds are triggered
static std::unordered_map<KEYBINDS_ENUM, std::vector<std::vector<int>>> keybinds = {
#ifdef DEBUG
    {DEBUG_WIREFRAME_ON, {{GLFW_KEY_F5}}},
    {DEBUG_WIREFRAME_OFF, {{GLFW_KEY_F6}}},
    {DEBUG_TICK, {{GLFW_KEY_T}}},
#endif
    {EXIT, {{GLFW_KEY_ESCAPE}}},
    {MOVE_UP, {{GLFW_KEY_W}, {GLFW_KEY_UP}}},
    {MOVE_DOWN, {{GLFW_KEY_S}, {GLFW_KEY_DOWN}}},
    {MOVE_LEFT, {{GLFW_KEY_A}, {GLFW_KEY_LEFT}}},
    {MOVE_RIGHT, {{GLFW_KEY_D}, {GLFW_KEY_RIGHT}}}
};

static std::unordered_map<MOUSE_KEYBINDS_ENUM, int> mouseKeybinds = {
    {CLICK_KEY, GLFW_MOUSE_BUTTON_LEFT},
    {DRAG_KEY, GLFW_MOUSE_BUTTON_MIDDLE}
};


#ifdef DEBUG
    constexpr auto logLevel = ErrorHandler::LOG_ALL;
#else
    constexpr auto logLevel = static_cast<ErrorHandler::LogLevel>(ErrorHandler::LOG_WARNING | ErrorHandler::LOG_ERROR);
#endif
ErrorHandler errorHandler(logLevel); // Global error handler

float scale = 1.0f;
vec2f offset;
Ticker ticker(&errorHandler);

// Keybind utilities
bool keyPressed(GLFWwindow* window, const KEYBINDS_ENUM key) {
    return std::ranges::any_of(keybinds[key], [&](const std::vector<int>& keybind) {
        return std::ranges::all_of(keybind, [&](const int k) { return glfwGetKey(window, k) == GLFW_PRESS; });
    });
}

bool mouseButtonPressed(GLFWwindow* window, const MOUSE_KEYBINDS_ENUM button) {
    return glfwGetMouseButton(window, mouseKeybinds[button]) == GLFW_PRESS;
}

bool mouseButtonReleased(GLFWwindow* window, const MOUSE_KEYBINDS_ENUM button) {
    return glfwGetMouseButton(window, mouseKeybinds[button]) == GLFW_RELEASE;
}

#ifdef DEBUG
bool tickButtonPressed = false;
#endif
void processInput(GLFWwindow* window) {
#ifdef DEBUG // Debug keybinds
    if (keyPressed(window, DEBUG_WIREFRAME_ON)) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (keyPressed(window, DEBUG_WIREFRAME_OFF)) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Debounce the key, so we only do one tick
    if (keyPressed(window, DEBUG_TICK) && !tickButtonPressed) ticker.tick();
    tickButtonPressed = keyPressed(window, DEBUG_TICK);
#endif

    // Exit on ESC
    if (keyPressed(window, EXIT)) glfwSetWindowShouldClose(window, true);

    // Move the view with WASD keys
    if (keyPressed(window, MOVE_UP)) offset.y += scale * 0.001f;
    if (keyPressed(window, MOVE_DOWN)) offset.y -= scale * 0.001f;
    if (keyPressed(window, MOVE_LEFT)) offset.x -= scale * 0.001f;
    if (keyPressed(window, MOVE_RIGHT)) offset.x += scale * 0.001f;
}

std::string selectedProv; // Currently selected province
void mouse_click_callback(GLFWwindow* window,
                          const int button,
                          const int action,
                          int mods) {
    if (!mouseButtonPressed(window, CLICK_KEY)) return;
    vec2d position;
    glfwGetCursorPos(window, &position.x, &position.y);
    vec2i dimensions;
    glfwGetWindowSize(window, &dimensions.x, &dimensions.y);
    vec2f f = static_cast<vec2f>(position) * 2.0f * scale / static_cast<vec2f>(dimensions);
    f = ((f - scale).negateY() + offset) / 0.95f;
    const auto *sm = static_cast<StateManager *>(glfwGetWindowUserPointer(window));
    if (const std::string state = sm->clickedOnState(f); !state.empty()) {
        const std::string provinceName = sm->pm->clickedOnProvince(f);
        const Province p = sm->pm->getProvince(provinceName);
        errorHandler.logDebug("Clicked on province: " + provinceName + ", on state: " + state);

#ifdef DEBUG
        for (auto adjacencyMap = sm->pm->getAdjacencyMap();
             const auto& adjProvName : adjacencyMap[provinceName]) {
            errorHandler.logDebug(" - Adjacent province: " + sm->pm->getProvince(adjProvName).getName());
        }
#endif

        if (selectedProv.empty()) {
            selectedProv = provinceName;
            errorHandler.logDebug("Selected " + selectedProv + " as the starting province for pathfinding.");
            return;
        }

        auto [steps, length, pathProvs] = sm->pm->findPath(selectedProv, provinceName);
        errorHandler.logDebug(selectedProv + " is connected to " + provinceName + " in: " + std::to_string(steps) + " steps.");
        errorHandler.logDebug("The length of this path is " + std::to_string(length));
        selectedProv = ""; // Reset selected province
#ifdef DEBUG
        if (steps <= 0) return; // Not connected or same province
        std::string path = " - Path: ";
        for (const auto &provName: pathProvs | std::views::keys)
            path += provName + (provName != provinceName ? " -> " : "");
        errorHandler.logDebug(path);
#endif
    }
}

void scroll_callback(GLFWwindow* window, const double xoffset, const double yoffset) {
    if (yoffset < 0.0 && scale <= 10.0f) scale += 0.1f;
    else if (scale >= 0.1f) scale -= 0.1f;
}

vec2f lastMousePos;
void mouse_cursor_callback(GLFWwindow* window, const double xpos, const double ypos) {
    const auto mousePos = static_cast<vec2f>(vec2d(xpos, ypos));
    if (mouseButtonReleased(window, DRAG_KEY)) {
        lastMousePos = mousePos;
        return;
    }

    const vec2f delta = mousePos - lastMousePos;
    if (delta.zero()) return; // No movement
    // Remember 1: We need to subtract the delta because the movement is inverted
    // Remember 2: The y-axis is inverted in window coordinates
    offset -= delta * scale * vec2f(0.002f, -0.002f);
    lastMousePos = mousePos;
}

int main() {
    const Window window(800, 600, "Caesar Engine", &errorHandler);

    StateManager sm(&errorHandler);
    ticker.registerTickCallback([&sm] { sm.tick(); });

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

        Window::clear(0.5f);
        processInput(window.window());

        // Setup shaders
        sm.pm->provShader.use();
        sm.pm->provShader.setFloat("scale", scale);
        sm.pm->provShader.setVec2f("offset", offset);

        sm.pm->textShader.use();
        sm.pm->textShader.setFloat("scale", scale);
        sm.pm->textShader.setVec2f("offset", offset);
        sm.pm->textShader.setInt("tex", 0);
        sm.pm->textShader.setFloat("alpha", scale < 0.5f ? scale + 0.5f : 1.0f);

        sm.pm->lineShader.use();
        sm.pm->lineShader.setFloat("scale", scale);
        sm.pm->lineShader.setVec2f("offset", offset);

        // Render states (and therefore provinces)
        sm.render(window, scale, offset);

        window.swapBuffers();
        glfwPollEvents();
    } return EXIT_SUCCESS;
}
