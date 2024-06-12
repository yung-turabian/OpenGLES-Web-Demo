#ifndef SHADER_H
#define SHADER_H

#include <cstddef>
#include <glad/glad.h>
#define GLM_FORCE_PURE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>
#include <fstream>
#include <sstream>

struct Shader {
  //program ID
  unsigned int ID; // gl Program object

  Shader() { } // empty construct, delete if you can

  Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertex_code;
    std::string fragment_code;
    std::ifstream vertex_shader_file;
    std::ifstream fragment_shader_file;

    vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
      vertex_shader_file.open(vertexPath);
      fragment_shader_file.open(fragmentPath);

      std::stringstream vertex_shader_stream, fragment_shader_stream;

      vertex_shader_stream << vertex_shader_file.rdbuf();
      fragment_shader_stream << fragment_shader_file.rdbuf();

      vertex_shader_file.close();
      fragment_shader_file.close();

      vertex_code = vertex_shader_stream.str();
      fragment_code = fragment_shader_stream.str();
    } catch(std::ifstream::failure e) {
      printf("[ogl shader] file not succesfully read.\n");
    }

    const char* vertex_shader_code = vertex_code.c_str();
    const char* fragment_shader_code = fragment_code.c_str();

    unsigned int vertex, fragment;
    // for compile errors
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex);
    // compile errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
      printf("[ogl shader] Error vertex shader compilation failed: \n"
                          "%s", infoLog);
      return;
    }
    
    // fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
      printf("[ogl shader] Error fragment shader compilation failed: \n"
                          "%s", infoLog);
      return;
    }

    
    ID = glCreateProgram();
    if(ID == 0) {
      printf("[ogl shader] Failed to create program object\n");
      return;
    }
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // check link status
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
      glGetProgramInfoLog(ID, 512, nullptr, infoLog);
      printf("[ogl shader] Error shader program linking failed: \n"
                          "%s", infoLog);
      glDeleteProgram(ID);
      return;
    }
    
    glDeleteShader(vertex); // clean up
    glDeleteShader(fragment);

  }
  
  // use/activate shader
  void use() {
    glUseProgram(ID);
  }

  void kill() {
    glDeleteProgram(ID);
  }

  // util uniform functions
  void setBool(const std::string &name, bool value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if(loc == -1) {
      printf("[ogl] Uniform location query failed for=  %s \n", name.c_str());
    } else {
      glUniform1i(loc, (int)value); 
    }
  }
  void setInt(const std::string &name, int value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if(loc == -1) {
      printf("[ogl] Uniform location query failed for=  %s \n", name.c_str());
    } else {
      glUniform1i(loc, value); 
    }
  }
  void setFloat(const std::string &name, float value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if(loc == -1) {
      printf("[ogl] Uniform location query failed for=  %s \n", name.c_str());
    } else {
      glUniform1f(loc, value); 
    }
  }
  void setMat4(const std::string &name, glm::mat4 value) const {
    GLint loc = glGetUniformLocation(ID, name.c_str());
    if(loc == -1) {
      printf("[ogl] Uniform location query failed for=  %s \n", name.c_str());
    }
    else {
      glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
  }

};
#endif