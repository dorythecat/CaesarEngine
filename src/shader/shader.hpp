#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>

#include "../utils.hpp"
#include "../error_handler/error_handler.h"

class ErrorHandler;

class Shader {
public:
  unsigned int ID{};

  explicit Shader(ErrorHandler* errorHandler) : errorHandler(errorHandler) {
    loadShaderCode("res/shaders/default.vert", "res/shaders/default.frag");
  }
  explicit Shader(ErrorHandler* errorHandler,
         const std::string &path) : errorHandler(errorHandler) {
    loadShaderCode(path + ".vert", path + ".frag");
  }
  Shader(ErrorHandler* errorHandler,
         const std::string &vertexPath,
         const std::string &fragmentPath) : errorHandler(errorHandler) {
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
  ErrorHandler* errorHandler;

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
      errorHandler->logError("Shader file not successfully read: " + std::string(e.what()),
        ErrorHandler::FILE_NOT_SUCCESSFULLY_READ_ERROR);
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

  void checkCompileErrors(const unsigned int shader, const std::string &type) const {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (success) return;
      glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
      errorHandler->logError("Shader compilation error: " + std::string(infoLog),
        ErrorHandler::SHADER_COMPILATION_ERROR);
    } else {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (success) return;
      glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
      errorHandler->logError("Program linking error: " + std::string(infoLog),
        ErrorHandler::PROGRAM_LINKING_ERROR);
    }
  }
};

#endif // SHADER_H
