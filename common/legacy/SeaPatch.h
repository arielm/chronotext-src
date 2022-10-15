#pragma once

#include "common/SeaSurface.h"

#include "chr/gl/Batch.h"

class SeaPatch
{
public:
  enum
  {
    MODE_UNDEFINED = 0,
    MODE_GRID_V = 1,
    MODE_GRID_H = 2,
    MODE_FILL = 4
  };

  SeaPatch() = default;
  SeaPatch(SeaSurface *surface);
  ~SeaPatch();

  void setFrontFace(GLenum mode);
  void setup(float ox, float oy, float w, float h, int gridSize, int mode);
  void update(float t, float swellFactor);

  void drawFill(const glm::vec4 &color);
  void drawGrid(const glm::vec4 &color);

protected:
  SeaSurface *surface;
  float ox, oy;
  int gridSize;
  int nx, ny;

  GLenum frontFace = chr::CCW;

  chr::gl::Buffer<chr::gl::Vertex<>> vertexBuffer;
  chr::gl::Buffer<GLuint> gridIndexBuffer;
  chr::gl::Buffer<GLuint> fillIndexBuffer;

  chr::gl::IndexedVertexBatch<> gridBatch;
  chr::gl::IndexedVertexBatch<> fillBatch;

  SeaSurface::Sample *samples;
};
