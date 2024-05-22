#pragma once

#include "common/SeaSurfaceSpiral2.h"
#include "common/xf/Font.h"

#include "chr/gl/Batch.h"

#include "Brick1.h"

class SeaBrickSpiral
{
public:
  std::vector<Brick1> bricks;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> vertexBuffer;
  chr::gl::Buffer<GLuint> capIndexBuffer;
  chr::gl::Buffer<GLuint> wallIndexBuffer;
  chr::gl::Buffer<GLuint> strokeIndexBuffer;

  SeaBrickSpiral() = default;
  SeaBrickSpiral(float brickHeight, float brickDepth, float brickPadding, float brickMargin);

  void update(SeaSurfaceSpiral2 &spiral);
  void drawText(chr::XFont &font, float fontSize, float polygonOffset = 0);

protected:
  float brickHeight;
  float brickDepth;
  float brickPadding;
  float brickMargin;

  void updateCapIndices();
  void updateWallIndices();
  void updateStrokeIndices();
  void updateVertices(SeaSurfaceSpiral2 &spiral);
};
