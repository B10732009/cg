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

  virtual bool load();
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
};

class ShadowProgram : public Program {
 public:
  ShadowProgram(Context *ctx);

  void doMainLoop() override;

 private:
  GLint SHADOW_MAP_SIZE = 1024;
  GLuint depthMapFBO;
};

class SkyboxProgram : public Program {
 public:
  SkyboxProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/skybox.vert";
    fragProgramFIle = "../assets/shaders/skybox.frag";
  }

  void doMainLoop() override;
};


class LightProgram : public Program {
 public:
  LightProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/light.vert";
    fragProgramFIle = "../assets/shaders/light.frag";
  }

  void doMainLoop() override;
};

class ShadowLightProgram : public Program {
 public:
  ShadowLightProgram(Context *ctx) : Program(ctx) {
    vertProgramFile = "../assets/shaders/shadowLight.vert";
    fragProgramFIle = "../assets/shaders/shadowLight.frag";
  }

  void doMainLoop() override;
};

class FilterProgram : public Program {
 public:
  FilterProgram(Context *ctx);

  void updateFrameBuffer(int SCR_WIDTH, int SCR_HEIGHT);
  void bindFrameBuffer();
  void doMainLoop() override;

 private:
  GLuint quadVAO;
  GLuint quadVBO[2];

  GLuint filterFBO;
  GLuint colorBuffer;
};

class FilterProgramBindFrameAdapter : public Program {
 public:
  FilterProgramBindFrameAdapter(Context *ctx, FilterProgram *filterProgram) : Program(ctx) { 
      p = filterProgram;
  }

  FilterProgram *p;

  bool load() override { return true; }
  void doMainLoop() override { p->bindFrameBuffer(); }
};