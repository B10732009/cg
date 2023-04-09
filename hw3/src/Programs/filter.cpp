#include <iostream>
#include "context.h"
#include "program.h"
#include "opengl_context.h"

FilterProgram::FilterProgram(Context *ctx) : Program(ctx) {
  vertProgramFile = "../assets/shaders/filter.vert";
  fragProgramFIle = "../assets/shaders/filter.frag";

  // TODO#3-1: Generate Framebuffer and VAO/VBO for filter
  // Note:     You need to design proper position/texcoord data for filter program (NDC)
  
  // FBO
  glGenFramebuffers(1, &filterFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, filterFBO);
  
  // VAO
  glGenVertexArrays(1, &quadVAO);
  glBindVertexArray(quadVAO);

  // VBO
  // set position and texture coordinates
  std::vector quad_pos = {
      -1.0f,  1.0f, 0.0f,
      -1.0f, -1.0f, 0.0f, 
       1.0f, -1.0f, 0.0f, 
      -1.0f,  1.0f, 0.0f, 
       1.0f, -1.0f, 0.0f, 
       1.0f,  1.0f, 0.0f
  };
  std::vector quad_tex = {
      0.0f, 1.0f, 
      0.0f, 0.0f, 
      1.0f, 0.0f, 
      0.0f, 1.0f, 
      1.0f, 0.0f, 
      1.0f, 1.0f
  };
  
  // generate VBO
  glGenBuffers(2, quadVBO);

  // position
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quad_pos.size(), quad_pos.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  // texture
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * quad_tex.size(), quad_tex.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  
  updateFrameBuffer(OpenGLContext::getWidth(), OpenGLContext::getHeight());
}

void FilterProgram::updateFrameBuffer(int SCR_WIDTH, int SCR_HEIGHT) {
  /* TODO#3-1: generate color/depth buffer for frame buffer
   *           (this function will also be trigger when windown resize)
   *           1. Generate color buffer texture and store in colorBuffer
   *           2. Set texture size to SCR_WIDTH*SCR_HEIGHT
   *           3. Set MIN/MAG filter to linear
   *           4. Generate/Bind a render buffer inad store in rboDepth
   *           5. Set Render buffer size to SCR_WIDTH*SCR_HEIGHT
   *           6. Attach colorBuffer and rboDepth to filterFBO
   * Hint:
   *           - glGenRenderbuffers
   *           - glBindRenderbuffer
   *           - glRenderbufferStorage
   *           - glBindFramebuffer
   *           - glFramebufferTexture2D
   *           - glFramebufferRenderbuffer
   */

  // frame buffer texture
  glGenTextures(1, &colorBuffer);
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
   
  // render buffer
  GLuint rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
  
  // check if the frame buffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "The frame buffer is not complete!" << std::endl;
}

void FilterProgram::bindFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, filterFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FilterProgram::doMainLoop() {
  glUseProgram(programId);

  /* TODO#3-1: pass VAO, enableEdgeDetection, eanbleGrayscale, colorBuffer to shader and render
  */

  // pass data to shader
  glBindFramebuffer(GL_FRAMEBUFFER, filterFBO);
  setInt("colorBuffer", 0);
  setInt("enableEdgeDetection", ctx->enableEdgeDetection);
  setInt("eanbleGrayscale", ctx->eanbleGrayscale);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // bind VAO
  glBindVertexArray(quadVAO);

  // bind texture and draw
  glBindTexture(GL_TEXTURE_2D, colorBuffer);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glUseProgram(0);
}