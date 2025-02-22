#version 460 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D tex;

float median(float r, float g, float b) {
  return max(min(r, g), min(max(r, g), b));
}

void main() {
  vec3 text_color = texture(tex, TexCoords).rgb;
  float d = median(text_color.r, text_color.g, text_color.b) - 0.5;
  float w = clamp((d / fwidth(d)) + 0.5, 0.0, 1.0);
  color = mix(vec4(0.0), vec4(1.0), w);
}
