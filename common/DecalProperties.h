#pragma once

#include "chr/gl/State.h"

namespace chr
{
  struct DecalProperties
  {
    gl::ShaderProgram shader;
    glm::vec4 color;
    gl::Texture texture;
    float zoom = 1;

    void assign(gl::State &state, gl::IndexedVertexBatch<gl::XYZ.UV> &batch)
    {
      state.setShader(shader);
      batch.setShaderColor(color);
      batch.setTexture(texture);
    }
  };
}
