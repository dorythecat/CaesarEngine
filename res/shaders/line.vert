#version 460 core
layout (location = 0) in vec2 aPos;

uniform float scale;
uniform vec2 offset;

void main() {
    gl_Position = vec4(aPos * 0.95 - offset, 0.0, scale);
}
