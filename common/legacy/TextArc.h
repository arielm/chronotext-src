#pragma once

#include "chr/gl/Matrix.h"
#include "chr/gl/Batch.h"

#include "common/LineProvider.h"

class TextArc
{
public:
  chr::gl::Matrix matrix;

  TextArc() = default;
  TextArc(std::shared_ptr<chr::XFont> font, std::shared_ptr<LineProvider> lineProvider, float lineHeight);

  void drawLine(float ox, float oy, float r, int lineIndex, float xm, float xp, const chr::math::Rectf &clip);
  void drawText(float xm, float ym, float w, float r, float a, float ox, float oy);

  void update(float x, float y, float w, float r, float a, float gridSize, float dd);
  void drawFill(const glm::vec4 &color);
  void drawGrid(const glm::vec4 &color);

protected:
  std::shared_ptr<chr::XFont> font;
  std::shared_ptr<LineProvider> lineProvider;
  float lineHeight;

  chr::gl::VertexBatch<> fillBatch;
  chr::gl::VertexBatch<> gridBatch;
};
