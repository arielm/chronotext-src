#pragma once

#include "WaveSurfaceLine.h"
#include "Brick3.h"

#include "common/xf/Font.h"

class WaveBrickLine
{
public:
  std::vector<Brick3> bricks;

  WaveSurfaceLine *waveLine;
  float lineLength;
  float segmentLength;

  float shift;

  WaveBrickLine(float brickHeight, float brickDepth, float brickPadding, float brickMargin);

  void updateIndices(std::vector<GLuint> &capIndices, std::vector<GLuint> &wallIndices, std::vector<GLuint> &strokeIndices);
  void updateVertices(std::vector<chr::gl::Vertex<>> &vertices, float scrollingOffset);
  void drawText(chr::XFont &font, float fontSize, float polygonOffset = 0);

protected:
  float brickHeight;
  float brickDepth;
  float brickPadding;
  float brickMargin;

  int verticesIndex;
};
