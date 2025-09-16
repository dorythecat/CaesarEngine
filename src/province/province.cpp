#include "province.hpp"

Province::Province(ErrorHandler* errorHandler,
                   const char* mapPath,
                   const Color color,
                   std::string name,
                   const City &city,
                   const std::unordered_set<Color, Color::HashFunction> &usedColors) :
city(city), color(color), name(std::move(name)), errorHandler(errorHandler)  {
  generateMesh(mapPath, usedColors);
  generateMeshData();
}

void Province::generateMesh(const char* mapPath, std::unordered_set<Color, Color::HashFunction> usedColors) {
  // If we don't do this, we'll get vertically flipped provinces
  stbi_set_flip_vertically_on_load(false);

  int x, y, n;
  unsigned char* data = stbi_load(mapPath, &x, &y, &n, 0);
  if (!data) {
    std::cout << "Failed to load texture" << std::endl;
    return;
  }

  // Even though the theoretical maximum size is more than this, it should NEVER
  // reach that point, so we can safely use this. Could probably reduce further, but, eh.
  vertices.reserve(static_cast<size_t>(x * y));
  indices.reserve(static_cast<size_t>(x * y));

  const float x1 = 2.0f / static_cast<float>(x);
  const float y1 = -2.0f / static_cast<float>(y);

  for (int i = 0; i < x * y * n; i += n) {
    if (data[i] != color.r ||
        data[i + 1] != color.g ||
        data[i + 2] != color.b) continue;

    // Some preliminary calculations we'll use over and over
    const int xy = i / n;
    const float p = static_cast<float>(xy % x) * x1 - 1.0f;
    const float q = static_cast<float>(xy / x) * y1 + 1.0f;
    area++;

    // Start of rectangle adjacency
    if (i % (n * x) > 0) {
      auto c = Color(data[i - n], data[i - n + 1], data[i - n + 2]);
      if (usedColors.contains(c)) adjacentColors.insert(c);
    }

    // Actual quad generation
    const int i0 = i;
    float p0 = p;
    while (data[i] == color.r &&
           data[i + 1] == color.g &&
           data[i + 2] == color.b &&
           i % (n * x) != 0) {
      p0 += x1;
      i += n;
      area++;
    }

    // End of rectangle adjacency
    if (i % (n * x) < n * (x - 1)) {
      auto c = Color(data[i + n], data[i + n + 1], data[i + n + 2]);
      if (usedColors.contains(c)) adjacentColors.insert(c);
    }

    // Above rectangle adjacency
    if (i >= n * x) {
      for (int j = i0 - n * x; j < i - n * x; j += n) {
        auto c = Color(data[j], data[j + 1], data[j + 2]);
        if (usedColors.contains(c)) adjacentColors.insert(c);
      }
    }

    // Below rectangle adjacency
    if (i <= n * x * (y - 1)) {
      for (int j = i0 + n * x; j < i + n * x; j += n) {
        auto c = Color(data[j], data[j + 1], data[j + 2]);
        if (usedColors.contains(c)) adjacentColors.insert(c);
      }
    }

    center += vec2f(p + p0, q + q + y1);

    addQuad(p, q, p0, q + y1, color);
  } stbi_image_free(data);

  adjacentColors.erase(color); // Remove the color of the province itself

  // Shrink to fit, so we don't waste any memory
  // And trust me, it will waste A LOT of memory
  vertices.shrink_to_fit();
  indices.shrink_to_fit();

#ifdef DEBUG
  std::cout << "Vertices: " << vertices.size() << std::endl;
  std::cout << "Indices: " << indices.size() << std::endl;
#endif

  center /= static_cast<float>(vertices.size());
}

void Province::generateMeshData() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // Bind VAO
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
               &vertices[0],
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
               &indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1,
                        3,
                        GL_UNSIGNED_BYTE,
                        GL_FALSE,
                        sizeof(Vertex),
                        reinterpret_cast<void *>(offsetof(Vertex, color)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Province::addQuad(const float x0, const float y0, const float x1, const float y1, const Color c) {
  const auto index = static_cast<unsigned int>(vertices.size());

  vertices.emplace_back(x0, y0, c);
  vertices.emplace_back(x0, y1, c);
  vertices.emplace_back(x1, y0, c);
  vertices.emplace_back(x1, y1, c);

  indices.push_back(index);
  indices.push_back(index + 1);
  indices.push_back(index + 2);

  indices.push_back(index + 3);
  indices.push_back(index + 2);
  indices.push_back(index + 1);
}

void Province::render() const {
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,
                 static_cast<int>(indices.size()),
                 GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);
}

bool Province::clickedOn(const float x, const float y) const {
  for (unsigned int i = 0; i < vertices.size(); i += 4)
    if (x >= vertices[i].x && x <= vertices[i + 3].x && y >= vertices[i + 3].y && y <= vertices[i].y) return true;
  return false;
}
