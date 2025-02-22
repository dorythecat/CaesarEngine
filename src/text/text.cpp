#include "text.hpp"

Text::Text(std::string atlasPath, std::string indexPath) {
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
    std::vector<float> data;
    while (getline(iss, line, ',')) data.push_back(std::stof(line));

    Character character;
    character.advance = data[1];
    character.quadLeft = data[2];
    character.quadBottom = data[3];
    character.quadRight = data[4];
    character.quadTop = data[5];
    character.atlasLeft = data[6] / static_cast<float>(width);
    character.atlasBottom = data[7] / static_cast<float>(height);
    character.atlasRight = data[8] / static_cast<float>(width);
    character.atlasTop = data[9] / static_cast<float>(height);
    characters.push_back(character);
  } index.close();

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

void Text::setText(std::string text, float x, float y, float scale) {
  size = text.size();
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
    Character character = characters[c - 32];

    if (character.quadLeft == character.quadRight ||
        character.quadBottom == character.quadTop) {
      xoffset += character.advance * scale;
      continue;
    };

    float x0 = xoffset + character.quadLeft * scale;
    float y0 = yoffset + character.quadBottom * scale;
    float x1 = xoffset + character.quadRight * scale;
    float y1 = yoffset + character.quadTop * scale;

    x0 = 2 * x0 / 800 - 1;
    y0 = 2 * y0 / 600 - 1;
    x1 = 2 * x1 / 800 - 1;
    y1 = 2 * y1 / 600 - 1;

    float s0 = character.atlasLeft;
    float t0 = character.atlasBottom;
    float s1 = character.atlasRight;
    float t1 = character.atlasTop;

    vertices[16 * i] = x0;
    vertices[16 * i + 1] = y0;
    vertices[16 * i + 2] = s0;
    vertices[16 * i + 3] = t0;

    vertices[16 * i + 4] = x1;
    vertices[16 * i + 5] = y0;
    vertices[16 * i + 6] = s1;
    vertices[16 * i + 7] = t0;

    vertices[16 * i + 8] = x1;
    vertices[16 * i + 9] = y1;
    vertices[16 * i + 10] = s1;
    vertices[16 * i + 11] = t1;

    vertices[16 * i + 12] = x0;
    vertices[16 * i + 13] = y1;
    vertices[16 * i + 14] = s0;
    vertices[16 * i + 15] = t1;

    indices[6 * i] = 4 * i;
    indices[6 * i + 1] = 4 * i + 1;
    indices[6 * i + 2] = 4 * i + 2;
    indices[6 * i + 3] = 4 * i;
    indices[6 * i + 4] = 4 * i + 2;
    indices[6 * i + 5] = 4 * i + 3;

    xoffset += character.advance * scale;
  }

  // No needs to shrink the vectors because the size is always the same

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(float),
               &vertices[0],
               GL_STATIC_DRAW);
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               indices.size() * sizeof(unsigned int),
               &indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Text::render() {
  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, atlas);
  glDrawElements(GL_TRIANGLES, 6 * size, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}
