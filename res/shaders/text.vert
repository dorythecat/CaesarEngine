#version 460 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;

uniform vec2 offset;
uniform float scale;

void main() {
  gl_Position = vec4(vertex - offset, 0.0, scale);
  TexCoords = texCoords;
}  
