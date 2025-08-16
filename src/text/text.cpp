#include "text.hpp"

Text::Text(const std::string &atlasPath, const std::string &indexPath) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrChannels;
  unsigned char *data = stbi_load(atlasPath.c_str(),
                                  &width,
                                  &height,
                                  &nrChannels,
                                  0);

  if (!data) {
    std::cerr << "Failed to load texture" << std::endl;
    return;
  }

  glGenTextures(1, &atlas);
  glBindTexture(GL_TEXTURE_2D, atlas);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGB,
               width,
               height,
               0,
               GL_RGB,
               GL_UNSIGNED_BYTE,
               data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  std::ifstream index(indexPath);
  if (!index.is_open()) {
    std::cerr << "Failed to open index file" << std::endl;
    return;
  }

  std::string line;
  while (std::getline(index, line)) {
    std::istringstream iss(line);
    std::vector<float> characterData;
    while (getline(iss, line, ',')) characterData.push_back(std::stof(line));

    Character character{
      characterData[1],
      characterData[2],
      characterData[3],
      characterData[4],
      characterData[5],
      characterData[6] / static_cast<float>(width),
      characterData[7] / static_cast<float>(height),
      characterData[8] / static_cast<float>(width),
      characterData[9] / static_cast<float>(height)
    };
    characters.push_back(character);
  }
  index.close();

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
}

Text::~Text() {
  glDeleteTextures(1, &atlas);
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

void Text::setText(const std::string &text,
                   const float x,
                   const float y,
                   const float scale,
                   const vec2i &windowDimensions) {
  size = static_cast<unsigned int>(text.size());
  std::vector<float> vertices(16 * size);
  std::vector<unsigned int> indices(6 * size);

  float xoffset = x;
  float yoffset = y;

  for (unsigned int i = 0; i < size; i++) {
    char c = text.c_str()[i];
    if (c == ' ') {
      xoffset += characters[0].advance * scale;
      continue;
    }
    if (c == '\n') {
      xoffset = x;
      yoffset -= scale;
      continue;
    }
    if (c == '\t') {
      xoffset += 4 * characters[0].advance * scale;
      continue;
    }
    if (c == '\r') {
      xoffset = x;
      continue;
    }
    if (c < 32 || c > 126) {
      xoffset += characters[0].advance * scale;
      continue;
    }
    auto [advance,
      quadLeft, quadBottom, quadRight, quadTop,
      atlasLeft, atlasBottom, atlasRight, atlasTop] = characters[static_cast<unsigned long>(c - 32)];

    if (quadLeft == quadRight ||
        quadBottom == quadTop) {
      xoffset += advance * scale;
      continue;
    };

    float x0 = xoffset + quadLeft * scale;
    float y0 = yoffset + quadBottom * scale;
    float x1 = xoffset + quadRight * scale;
    float y1 = yoffset + quadTop * scale;

    x0 = 2.0f * x0 / static_cast<float>(windowDimensions.x) - 1.0f;
    y0 = 2.0f * y0 / static_cast<float>(windowDimensions.y) - 1.0f;
    x1 = 2.0f * x1 / static_cast<float>(windowDimensions.x) - 1.0f;
    y1 = 2.0f * y1 / static_cast<float>(windowDimensions.y) - 1.0f;

    vertices[16 * i] = x0;
    vertices[16 * i + 1] = y0;
    vertices[16 * i + 2] = atlasLeft;
    vertices[16 * i + 3] = atlasBottom;

    vertices[16 * i + 4] = x1;
    vertices[16 * i + 5] = y0;
    vertices[16 * i + 6] = atlasRight;
    vertices[16 * i + 7] = atlasBottom;

    vertices[16 * i + 8] = x1;
    vertices[16 * i + 9] = y1;
    vertices[16 * i + 10] = atlasRight;
    vertices[16 * i + 11] = atlasTop;

    vertices[16 * i + 12] = x0;
    vertices[16 * i + 13] = y1;
    vertices[16 * i + 14] = atlasLeft;
    vertices[16 * i + 15] = atlasTop;

    indices[6 * i] = 4 * i;
    indices[6 * i + 1] = 4 * i + 1;
    indices[6 * i + 2] = 4 * i + 2;
    indices[6 * i + 3] = 4 * i;
    indices[6 * i + 4] = 4 * i + 2;
    indices[6 * i + 5] = 4 * i + 3;

    xoffset += advance * scale;
  }

  // No needs to shrink the vectors because the size is always the same

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
               &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(unsigned int)),
               &indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void *>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Text::render() const {
  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, atlas);
  glDrawElements(GL_TRIANGLES, static_cast<GLint>(6 * size), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
