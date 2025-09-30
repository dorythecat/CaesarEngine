#version 460 core
layout (location = 0) in vec2 aPos;

out vec3 ourColor;

uniform float scale;
uniform vec2 offset;

void main() {
  gl_Position = vec4(aPos - offset, 0.0, scale);
  ourColor = vec3(0.7, 0.3, 0.2);
}
