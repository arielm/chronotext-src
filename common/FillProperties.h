#pragma once

#include "chr/gl/State.h"

namespace chr
{
  struct FillProperties
  {
    gl::ShaderProgram shader;
    glm::vec4 color;

    void assign(gl::State &state, gl::IndexedVertexBatch<> &batch)
    {
      state.setShader(shader);
      batch.setShaderColor(color);
    }
  };
}
