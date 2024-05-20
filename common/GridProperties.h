#pragma once

#include "chr/gl/State.h"

namespace chr
{
  struct GridProperties
  {
    gl::ShaderProgram shader;
    glm::vec4 color;
    float lineWidth;
    float gridSize;
    bool horizontal;
    bool vertical;

    void assign(gl::State &state, gl::VertexBatch<> &batch)
    {
      state.setShader(shader);
      state.glLineWidth(lineWidth);
      batch.setShaderColor(color);
    }
  };
}
