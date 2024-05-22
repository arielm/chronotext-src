#pragma once

#include "WaveSurfaceLine.h"
#include "Brick3.h"

#include "common/xf/Font.h"

class ThinWaveBrickLine
{
public:
  std::vector<Brick3> bricks;

  WaveSurfaceLine *waveLine;
  float lineLength;
  float segmentLength;

  float shift;

  ThinWaveBrickLine(float brickHeight, float brickPadding, float brickMargin);

  void updateIndices(std::vector<GLuint> &strokeIndices);
  void updateVertices(std::vector<chr::gl::Vertex<>> &vertices, float scrollingOffset);
  void drawText(chr::XFont &font, float fontSize, float polygonOffset = 0);

protected:
  float brickHeight;
  float brickPadding;
  float brickMargin;

  int verticesIndex;
};
