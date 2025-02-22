#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aColor;

out vec3 ourColor;

uniform float scale;
uniform vec2 offset;
uniform sampler2D heightmap;

void main() {
  float h = texture(heightmap, aTexCoords).r;
  gl_Position = vec4(aPos - offset, h, scale);
  ourColor = aColor;
}
