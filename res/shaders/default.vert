#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

uniform float scale;
uniform vec2 offset;

void main() {
  gl_Position = vec4(aPos - offset / 10.0, 0.0, scale);
  ourColor = aColor;
}
