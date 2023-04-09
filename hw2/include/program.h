#pragma once

#include <glad/gl.h>
#include "gl_helper.h"

class Context;

class Program {
 public:
  const char *vertProgramFile;
  const char *fragProgramFIle;

 public:
  Program(Context *ctx) : ctx(ctx) {
    vertProgramFile = "../assets/shaders/example.vert";
    fragProgramFIle = "../assets/shaders/example.frag";
  }

  virtual bool load() = 0;
  virtual void doMainLoop() = 0;

  void setMat4(const char *varname, const float *data) {
    GLint loc = glGetUniformLocation(programId, varname);
    glUniformMatrix4fv(loc, 1, GL_FALSE, data);
  }
  void setVec3(const char *varname, const float *data) {
    GLint loc = glGetUniformLocation(programId, varname);
    glUniform3fv(loc, 1, data);
  }
  void setFloat(const char *varname, const float data) {
    GLint loc = glGetUniformLocation(programId, varname);
    glUniform1f(loc, data);
  }
  void setInt(const char *varname, const int data) {
    GLint loc = glGetUniformLocation(programId, varname);
    glUniform1i(loc, data);
  }

 protected:
  GLuint programId = -1;
  const Context *ctx;
  GLuint *VAO = 0;
};

class ExampleProgram : public Program {
 public:
  ExampleProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/example.vert";
    fragProgramFIle = "../assets/shaders/example.frag";
  }
  bool load() override;
  void doMainLoop() override;
};

class BasicProgram : public Program {
 public:
  BasicProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/basic.vert";
    fragProgramFIle = "../assets/shaders/basic.frag";
  }
  bool load() override;
  void doMainLoop() override;
};

class LightProgram : public Program {
 public:
  LightProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/light.vert";
    fragProgramFIle = "../assets/shaders/light.frag";
  }
  bool load() override;
  void doMainLoop() override;
};
