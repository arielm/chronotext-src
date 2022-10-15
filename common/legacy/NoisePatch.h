#pragma once

#include "common/NoiseSurface.h"

#include "chr/gl/Batch.h"

class NoisePatch
{
public:
  enum
  {
    MODE_UNDEFINED = 0,
    MODE_GRID_V = 1,
    MODE_GRID_H = 2,
    MODE_FILL = 4
  };

  NoisePatch() = default;
  NoisePatch(NoiseSurface *surface);

  void setFrontFace(GLenum mode);
  void setup(float cx, float cy, float w, float h, int mode);
  void update(float height, const glm::vec2 &offset);

  void drawFill(const glm::vec4 &color);
  void drawGrid(const glm::vec4 &color);

protected:
  NoiseSurface *surface;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> vertexBuffer;
  chr::gl::Buffer<GLuint> gridIndexBuffer;
  chr::gl::Buffer<GLuint> fillIndexBuffer;

  chr::gl::IndexedVertexBatch<> gridBatch;
  chr::gl::IndexedVertexBatch<> fillBatch;

  GLenum frontFace = chr::CCW;

  int nx, ny;
  float ox1, oy1;
  float ox2, oy2;
};
