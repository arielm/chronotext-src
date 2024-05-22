#pragma once

#include "Brick3.h"

#include "common/xf/Font.h"

class ThinFlatBrickLine
{
public:
  std::vector<Brick3> bricks;

  float y;
  float shift;

  ThinFlatBrickLine(float brickHeight, float brickPadding, float brickMargin);

  void updateIndices(std::vector<GLuint> &strokeIndices);
  void updateVertices(std::vector<chr::gl::Vertex<>> &vertices, float scrollingOffset);
  void drawText(chr::XFont &font, float fontSize);

protected:
  float brickHeight;
  float brickPadding;
  float brickMargin;

  int verticesIndex;
};
