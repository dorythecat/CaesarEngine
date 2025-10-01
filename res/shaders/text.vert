#version 460 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

uniform vec2 offset;
uniform float scale;
uniform vec2 center;

void main() {
  gl_Position = vec4((vertex - center) * 0.9 + center - offset, 0.0, scale);
  TexCoords = texCoords;
}  
