#pragma once

#include "Brick2.h"

#include "common/xf/Font.h"

class BrickLine
{
public:
  std::vector<Brick2> bricks;

  float ox;
  float oy;
  float shift;

  BrickLine(float brickHeight, float brickDepth, float brickPadding);

  void updateIndices(std::vector<GLuint> &capIndices, std::vector<GLuint> &wallIndices, std::vector<GLuint> &strokeIndices);
  void updateVertices(std::vector<chr::gl::Vertex<>> &vertices);
  void drawText(chr::XFont &font, float fontSize);

protected:
  float brickHeight;
  float brickDepth;
  float brickPadding;

  int verticesIndex;
};
