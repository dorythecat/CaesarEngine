#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../utils.hpp"

class Shader {
public:
  unsigned int ID{};

  Shader() {
    loadShaderCode("res/shaders/default.vert", "res/shaders/default.frag");
  }
  explicit Shader(const std::string &path) {
    loadShaderCode(path + ".vert", path + ".frag");
  }
  Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    loadShaderCode(vertexPath, fragmentPath);
  }

  void use() const { glUseProgram(ID); }

  // --- Utility uniform functions ---
  // Boolean
  void setBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
  }
  // Scalars
  void setInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
  }
  void setFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
  }
  void setDouble(const std::string &name, const double value) const {
    glUniform1d(glGetUniformLocation(ID, name.c_str()), value); 
  }
  // Vectors
  void setVec2f(const std::string &name,
                const float x,
                const float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
  }
  void setVec2f(const std::string &name,
                const vec2f &v) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), v.x, v.y);
  }
  void setVec3f(const std::string &name,
                const float x,
                const float y,
                const float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
  }
  void setVec4f(const std::string &name,
                const float x,
                const float y,
                const float z,
                const float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
  }

private:
  void loadShaderCode(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;

      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      
      vShaderFile.close();
      fShaderFile.close();
      
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    unsigned int vertex, fragment;
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
  }

  static void checkCompileErrors(const unsigned int shader, const std::string &type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (success) return;
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog <<
        "\n -- --------------------------------------------------- -- " << std::endl;
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (success) return;
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog <<
        "\n -- --------------------------------------------------- -- " << std::endl;
    }
  }
};

#endif // SHADER_H
