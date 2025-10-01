#version 460 core
layout (location = 0) in vec2 aPos;

uniform float scale;
uniform vec2 offset;
uniform vec3 color;
uniform vec2 center;

void main() {
  // Make the shape scale around its center
  gl_Position = vec4((aPos - center) * 0.9 + center - offset, 0.0, scale);
}
