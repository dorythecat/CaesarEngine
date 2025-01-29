#version 460 core
in vec3 ourColor;

out vec4 fragColor;

uniform float scale;

void main() {
  fragColor = vec4(ourColor, 1.0);
}
