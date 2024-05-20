#include "common/TextRectangle.h"

#include "chr/utils/Utils.h"
#include "chr/math/Utils.h"

using namespace std;
using namespace chr::math;
using namespace chr::gl;
using namespace chr::utils;

namespace chr
{
  void TextRectangle::setDecalProperties(const DecalProperties &properties)
  {
    decalProperties = properties;
  }

  void TextRectangle::setFillProperties(const FillProperties &properties)
  {
    fillProperties = properties;
  }

  void TextRectangle::setGridProperties(const GridProperties &properties)
  {
    gridProperties = properties;
  }

  void TextRectangle::setTextProperties(const TextProperties &properties)
  {
    textProperties = properties;
  }

  void TextRectangle::drawGrid(VertexBatch<> &batch, const Matrix &matrix, float x, float y, float w, float h, float ox, float oy)
  {
    float x2 = x + w;
    float y2 = y + h;

    float ox1 = floorf(x / gridProperties.gridSize);
    float ox2 = ceilf(x2 / gridProperties.gridSize);

    float oy1 = floorf(y / gridProperties.gridSize);
    float oy2 = ceilf(y2 / gridProperties.gridSize);

    int nx = int(ox2 - ox1) + 1;
    int ny = int(oy2 - oy1) + 1;

    // ---

    // TODO: batch.reserve((nx + ny) * 2);

    if (gridProperties.vertical)
    {
      for (int ix = 0; ix < nx; ix++)
      {
        float xx = x + gridProperties.gridSize * ix - boundf(ox, gridProperties.gridSize);

        if ((xx >= x) && (xx < x2))
        {
          batch.addVertices(matrix.transformPoint(xx, y), matrix.transformPoint(xx, y2));
        }
      }
    }

    if (gridProperties.horizontal)
    {
      for (int iy = 0; iy < ny; iy++)
      {
        float yy = y + gridProperties.gridSize * iy - boundf(oy, gridProperties.gridSize);

        if ((yy >= y) && (yy < y2))
        {
          batch.addVertices(matrix.transformPoint(x, yy), matrix.transformPoint(x2, yy));
        }
      }
    }
  }

  void TextRectangle::drawFill(IndexedVertexBatch<> &batch, const Matrix &matrix, float x, float y, float w, float h)
  {
    matrix.addTransformedQuad(Quad<>(x, y, x + w, y + h), batch);
  }

  void TextRectangle::drawDecal(IndexedVertexBatch<XYZ.UV> &batch, const Matrix &matrix, float x, float y, float w, float h, float ox, float oy)
  {
    float x2 = x + w;
    float y2 = y + h;

    float W = decalProperties.texture.width * decalProperties.zoom;
    float H = decalProperties.texture.height * decalProperties.zoom;

    float u1 = (x + ox) / W;
    float v1 = (y + oy) / H;
    float u2 = (x2 + ox) / W;
    float v2 = (y2 + oy) / H;

    matrix.addTransformedQuad(Quad<XYZ.UV>(x, y, x2, y2, u1, v1, u2, v2), batch);
  }

  void TextRectangle::drawText(const Matrix &matrix, float x, float y, float w, float h, float ox, float oy)
  {
    textProperties.font->setSize(textProperties.fontSize);
    textProperties.font->setColor(textProperties.color);

    float xp = x + w;
    float yp = y + h;

    textProperties.font->setClip(x, y, xp, yp);

    float baselineOffset = -textProperties.font->getOffsetY(XFont::ALIGN_MIDDLE);
    float minus = y + (baselineOffset + textProperties.font->getAscent()) - fmodf(oy, textProperties.lineHeight);
    float plus = yp - (baselineOffset - textProperties.font->getDescent()) - fmodf(oy, textProperties.lineHeight);

    int total_m = int(minus / textProperties.lineHeight);
    int total_p = int(plus / textProperties.lineHeight);

    // ---

    for (int i = total_m; i <= total_p; i++)
    {
      drawLine(*textProperties.font, *textProperties.lineProvider, ox, i * textProperties.lineHeight + fmodf(oy, textProperties.lineHeight) + baselineOffset, bound(int(oy / textProperties.lineHeight) - i, textProperties.lineProvider->size), x, xp, matrix);
    }

    textProperties.font->clearClip();
  }

  void TextRectangle::drawLine(XFont &font, LineProvider &lineProvider, float ox, float oy, int lineIndex, float xm, float xp, const Matrix &matrix)
  {
    float total = lineProvider.linesLen[lineProvider.size];

    float bound_begin;
    float bound_end;
    float o_begin = 0;

    ox += font.getDirection() * lineProvider.linesLen[lineIndex];

    if (font.getDirection() > 0)
    {
      bound_begin = xm + ox;
      bound_end = xp;

      if (bound_begin < 0)
      {
        o_begin = total * int(bound_begin / total) - total;
        bound_begin = boundf(bound_begin, total);
      }
      else if (bound_begin >= total)
      {
        o_begin = total * int(bound_begin / total);
        bound_begin = fmodf(bound_begin, total);
      }
    }
    else
    {
      bound_begin = xp + ox;
      bound_end = xm;

      if (bound_begin < 0)
      {
        o_begin = -total * int(bound_begin / total);
        bound_begin = -fmodf(bound_begin, total);
      }
      else
      {
        o_begin = -total * int(bound_begin / total) - total;
        bound_begin = total - fmodf(bound_begin, total);
      }
    }

    auto index_line_begin = search(lineProvider.linesLen, bound_begin, 1, lineProvider.linesLen.size());
    auto index_char_begin = search(lineProvider.charsLen[index_line_begin], bound_begin, 1, lineProvider.charsLen[index_line_begin].size());
    float offset_begin = font.getDirection() * (o_begin + lineProvider.charsLen[index_line_begin][index_char_begin]) - ox;

    // ---

    int size = lineProvider.lines[index_line_begin].size();

    do
    {
      int glyphIndex = font.getGlyphIndex((index_char_begin == size) ? u' ' : lineProvider.lines[index_line_begin][index_char_begin]);
      font.addGlyph(matrix, glyphIndex, offset_begin, oy);
      offset_begin += font.getDirection() * font.getGlyphAdvance(glyphIndex);

      if (++index_char_begin > size)
      {
        index_char_begin = 0;
        index_line_begin = (index_line_begin + 1) % lineProvider.size;

        size = lineProvider.lines[index_line_begin].size();
      }
    }
    while ((font.getDirection() > 0) ? (offset_begin < bound_end) : (offset_begin > bound_end));
  }
}
