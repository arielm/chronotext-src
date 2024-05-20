#pragma once

#include "chr/gl/Batch.h"

#include "common/GridProperties.h"
#include "common/FillProperties.h"
#include "common/DecalProperties.h"
#include "common/TextProperties.h"

namespace chr
{
  class TextRectangle
  {
  public:
    DecalProperties decalProperties;

    void setDecalProperties(const DecalProperties &properties);
    void setFillProperties(const FillProperties &properties);
    void setGridProperties(const GridProperties &properties);
    void setTextProperties(const TextProperties &properties);

    void drawGrid(gl::VertexBatch<> &batch, const gl::Matrix &matrix, float x, float y, float w, float h, float ox = 0, float oy = 0);
    void drawFill(gl::IndexedVertexBatch<> &batch, const gl::Matrix &matrix, float x, float y, float w, float h);
    void drawDecal(gl::IndexedVertexBatch<gl::XYZ.UV> &batch, const gl::Matrix &matrix, float x, float y, float w, float h, float ox = 0, float oy = 0);
    void drawText(const gl::Matrix &matrix, float x, float y, float w, float h, float ox = 0, float oy = 0);

  protected:
    FillProperties fillProperties;
    GridProperties gridProperties;
    TextProperties textProperties;

    void drawLine(XFont &font, LineProvider &lineProvider, float ox, float oy, int lineIndex, float xm, float xp, const gl::Matrix &matrix);
  };
}
