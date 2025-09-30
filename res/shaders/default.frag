#version 460 core
out vec4 fragColor;

uniform float scale;
uniform vec2 offset;
uniform vec3 color;

void main() {
  fragColor = vec4(color, 1.0);
}
