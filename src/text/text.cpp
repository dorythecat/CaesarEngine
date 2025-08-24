#include "text.hpp"

Text::Text(ErrorHandler* errorHandler, const std::string &atlasPath, const std::string &indexPath) :
errorHandler(errorHandler) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrChannels;
  unsigned char *data = stbi_load(atlasPath.c_str(),
                                  &width,
                                  &height,
                                  &nrChannels,
                                  0);

  if (!data) {
    errorHandler->logError("Failed to load texture atlas from " + atlasPath, ErrorHandler::COULD_NOT_OPEN_FILE_ERROR);
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
    errorHandler->logError("Failed to load texture index from " + indexPath, ErrorHandler::COULD_NOT_OPEN_FILE_ERROR);
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
                   const float scale,
                   vec2f position,
                   const vec2f &windowDimensions,
                   const vec2f &offset) {
  size = 0;
  std::vector<float> vertices;
  std::vector<unsigned int> indices;

  position = (position + 0.5f) * windowDimensions - vec2f(scale, 0.0f);

  vec2f textOffset = position;

  for (unsigned int i = 0; i < text.size(); i++) {
    const char c = text.c_str()[i];
    if (c == ' ') {
      textOffset.x += characters[0].advance * scale;
      continue;
    }
    if (c == '\n') {
      textOffset.x = position.x;
      textOffset.y -= scale;
      continue;
    }
    if (c == '\t') {
      textOffset.x += 4 * characters[0].advance * scale;
      continue;
    }
    if (c == '\r') {
      textOffset.x = position.x;
      continue;
    }
    if (c < 32 || c > 126) {
      textOffset.x += characters[0].advance * scale;
      continue;
    }
    auto [advance,
      quadLeft, quadBottom, quadRight, quadTop,
      atlasLeft, atlasBottom, atlasRight, atlasTop] = characters[static_cast<unsigned long>(c - 32)];

    if (quadLeft == quadRight ||
        quadBottom == quadTop) {
      textOffset.x += advance * scale;
      continue;
    };

    const vec2f q0 = (textOffset + vec2f(quadLeft, quadBottom) * scale) * 2.0f / windowDimensions- 1.0f;
    const vec2f q1 = (textOffset + vec2f(quadRight, quadTop) * scale) * 2.0f / windowDimensions - 1.0f;


    vertices.insert(vertices.end(), {
      q0.x, q0.y, atlasLeft, atlasBottom,
      q1.x, q0.y, atlasRight, atlasBottom,
      q1.x, q1.y, atlasRight, atlasTop,
      q0.x, q1.y, atlasLeft, atlasTop
    });

    indices.insert(indices.end(), {
      4 * size, 4 * size + 1, 4 * size + 2,
      4 * size, 4 * size + 2, 4 * size + 3
    });

    textOffset.x += advance * scale;
    size++;
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
