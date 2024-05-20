#pragma once

#include "chr/gl/Matrix.h"
#include "chr/gl/Batch.h"

#include "common/LineProvider.h"

class TextRectangle
{
public:
  chr::gl::Matrix matrix;

  TextRectangle() = default;
  TextRectangle(std::shared_ptr<chr::XFont> font, std::shared_ptr<LineProvider> lineProvider, float lineHeight);

  void drawLine(float ox, float oy, int lineIndex, float xm, float xp);
  void drawText(float xm, float ym, float w, float h, float ox, float oy);

  void update(float x, float y, float w, float h, float gridSize);
  void drawFill(const glm::vec4 &color);
  void drawGrid(const glm::vec4 &color);

protected:
  std::shared_ptr<chr::XFont> font;
  std::shared_ptr<LineProvider> lineProvider;
  float lineHeight;

  chr::gl::VertexBatch<> fillBatch;
  chr::gl::VertexBatch<> gridBatch;
};
