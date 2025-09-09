#version 460 core
out vec4 fragColor;

uniform float scale;
uniform vec2 offset;

void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black color for lines
}