#pragma once

#include "chr/gl/Batch.h"

#include "common/xf/Font.h"

class Helix
{
public:
  chr::gl::VertexBatch<> wireBatch;

  Helix() = default;
  Helix(std::shared_ptr<chr::xf::Font> font);

  void update(float r1, float r2, float turns, float h, float DD1, float DD2);
  void drawText(const std::u16string &text, float r1, float r2, float turns, float h, float D, float size1, float size2);

protected:
  std::shared_ptr<chr::xf::Font> font;
};
