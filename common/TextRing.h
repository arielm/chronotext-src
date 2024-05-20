#pragma once

#include "chr/gl/Batch.h"

#include "common/GridProperties.h"
#include "common/FillProperties.h"
#include "common/DecalProperties.h"
#include "common/TextProperties.h"

namespace chr
{
  class TextRing
  {
  public:
    DecalProperties decalProperties;

    void setDecalProperties(const DecalProperties &properties);
    void setFillProperties(const FillProperties &properties);
    void setGridProperties(const GridProperties &properties);
    void setTextProperties(const TextProperties &properties);

    void drawGrid(gl::VertexBatch<> &batch, const gl::Matrix &matrix, float x, float y, float h, float r, float a, float dd, float ox = 0, float oy = 0);
    void drawFill(gl::IndexedVertexBatch<> &batch, const gl::Matrix &matrix, float x, float y, float h, float r, float a, float dd);
    void drawDecal(gl::IndexedVertexBatch<gl::XYZ.UV> &batch, const gl::Matrix &matrix, float x, float y, float h, float r, float a, float dd, float ox = 0, float oy = 0);
    void drawText(const gl::Matrix &matrix, float x, float y, float h, float r, float a, float ox = 0, float oy = 0);

  protected:
    FillProperties fillProperties;
    GridProperties gridProperties;
    TextProperties textProperties;

    void drawLine(XFont &font, LineProvider &lineProvider, float ox, float oy, float r, int lineIndex, float xm, float xp, const gl::Matrix &matrix, const math::Rectf &clip);
  };
}
