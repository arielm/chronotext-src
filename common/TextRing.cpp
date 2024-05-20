#include "common/TextRing.h"

#include "chr/utils/Utils.h"
#include "chr/math/Utils.h"

using namespace std;
using namespace chr::gl;
using namespace chr::math;
using namespace chr::utils;

namespace chr
{
  void TextRing::setDecalProperties(const DecalProperties &properties)
  {
    decalProperties = properties;
  }

  void TextRing::setFillProperties(const FillProperties &properties)
  {
    fillProperties = properties;
  }

  void TextRing::setGridProperties(const GridProperties &properties)
  {
    gridProperties = properties;
  }

  void TextRing::setTextProperties(const TextProperties &properties)
  {
    textProperties = properties;
  }

  void TextRing::drawGrid(gl::VertexBatch<> &batch, const Matrix &matrix, float x, float y, float h, float r, float a, float dd, float ox, float oy)
  {
    float y2 = y + h;

    float oy1 = floorf(y / gridProperties.gridSize);
    float oy2 = ceilf(y2 / gridProperties.gridSize);
    int ny = int(oy2 - oy1);

    int n1 = ceilf(a * r / dd);
    int n2 = ceilf(a * r / gridProperties.gridSize) + 1;

    // ---

    // TODO: batch.reserve((ny * n1 + n2) * 2);

    if (gridProperties.vertical)
    {
      for (int i = 0; i < n2; i++)
      {
        float d = (i * gridProperties.gridSize - boundf(ox, gridProperties.gridSize)) / r;
        float xx = x + sinf(d) * r;
        float zz = cosf(d) * r;

        if ((d >= 0) && (d < a))
        {
          batch.addVertices(matrix.transformPoint(xx, y, zz), matrix.transformPoint(xx, y2, zz));
        }
      }
    }

    if (gridProperties.horizontal)
    {
      for (int iy = 0; iy <= ny; iy++)
      {
        float yy = y + gridProperties.gridSize * iy - boundf(oy, gridProperties.gridSize);

        if ((yy >= y) && (yy < y2))
        {
          for (int i = 0; i < n1; i++)
          {
            float d1 = min(a, i * dd / r);
            float xx1 = x + sinf(d1) * r;
            float zz1 = cosf(d1) * r;

            float d2 = min(a, (i + 1) * dd / r);
            float xx2 = x + sinf(d2) * r;
            float zz2 = cosf(d2) * r;

            batch.addVertices(matrix.transformPoint(xx1, yy, zz1), matrix.transformPoint(xx2, yy, zz2));
          }
        }
      }
    }
  }

  void TextRing::drawFill(IndexedVertexBatch<> &batch, const Matrix &matrix, float x, float y, float h, float r, float a, float dd)
  {
    float w = a * r;
    float y2 = y + h;

    int n = ceilf(w / dd) + 1;

    // TODO: batch.reserve(n * 2);

    for (int i = 0; i < n; i++)
    {
      float d = min(a, i * dd / r);
      float xx = x + sinf(d) * r;
      float zz = cosf(d) * r;

      batch.addVertices(matrix.transformPoint(xx, y, zz), matrix.transformPoint(xx, y2, zz));

      if (i < n - 1)
      {
        batch.addIndices(0, 1, 3, 3, 2, 0);
      }

      batch.incrementIndices(2);
    }
  }

  void TextRing::drawDecal(IndexedVertexBatch<XYZ.UV> &batch, const Matrix &matrix, float x, float y, float h, float r, float a, float dd, float ox, float oy)
  {
    float w = a * r;
    float x2 = x + w;
    float y2 = y + h;

    float W = decalProperties.texture.width * decalProperties.zoom;
    float H = decalProperties.texture.height * decalProperties.zoom;

    float u1 = (x + ox) / W;
    float v1 = (y + oy) / H;
    float u2 = (x2 + ox) / W;
    float v2 = (y2 + oy) / H;

    int n = ceilf(w / dd) + 1;

    // TODO: batch.reserve(n * 2);

    for (int i = 0; i < n; i++)
    {
      float d = min(a, i * dd / r);
      float u = u1 + (u2 - u1) * d / a;
      float xx = x + sinf(d) * r;
      float zz = cosf(d) * r;

      batch
        .addVertex(matrix.transformPoint(xx, y, zz), u, v1)
        .addVertex(matrix.transformPoint(xx, y2, zz), u, v2);

      if (i < n - 1)
      {
        batch.addIndices(0, 1, 3, 3, 2, 0);
      }

      batch.incrementIndices(2);
    }
  }

  void TextRing::drawText(const Matrix &matrix, float x, float y, float h, float r, float a, float ox, float oy)
  {
    textProperties.font->setSize(textProperties.fontSize);
    textProperties.font->setColor(textProperties.color);

    float xp = x + a * r;
    float yp = y + h;

    Rectf clip(glm::vec2(x, y), glm::vec2(xp, yp));

    float baselineOffset = -textProperties.font->getOffsetY(XFont::ALIGN_MIDDLE);
    float minus = y + (baselineOffset + textProperties.font->getAscent()) - fmodf(oy, textProperties.lineHeight);
    float plus = yp - (baselineOffset - textProperties.font->getDescent()) - fmodf(oy, textProperties.lineHeight);

    int total_m = int(minus / textProperties.lineHeight);
    int total_p = int(plus / textProperties.lineHeight);

    // ---

    for (int i = total_m; i <= total_p; i++)
    {
      drawLine(*textProperties.font, *textProperties.lineProvider, ox, i * textProperties.lineHeight + fmodf(oy, textProperties.lineHeight) + baselineOffset, r, bound(int(oy / textProperties.lineHeight) - i, textProperties.lineProvider->size), x, xp, matrix, clip);
    }
  }

  void TextRing::drawLine(XFont &font, LineProvider &lineProvider, float ox, float oy, float r, int lineIndex, float xm, float xp, const Matrix &matrix, const Rectf &clip)
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

    Matrix fontMatrix;
    int size = lineProvider.lines[index_line_begin].size();

    do
    {
      int glyphIndex = font.getGlyphIndex((index_char_begin == size) ? u' ' : lineProvider.lines[index_line_begin][index_char_begin]);
      float half = font.getDirection() * font.getGlyphAdvance(glyphIndex) * 0.5f;
      offset_begin += half;

      float d = (offset_begin - xm) / r;
      float xx = sinf(d) * r;
      float zz = cosf(d) * r;

      fontMatrix.set(matrix);
      fontMatrix.translate(xm + xx, oy, zz);
      fontMatrix.rotateY(d);

      font.setClip(clip.x1 - d * r - xm, clip.y1 - oy, clip.x2 - d * r - xm, clip.y2 - oy);
      font.addGlyph(fontMatrix, glyphIndex, -half, 0);
      offset_begin += half;

      if (++index_char_begin > size)
      {
        index_char_begin = 0;
        index_line_begin = (index_line_begin + 1) % lineProvider.size;

        size = lineProvider.lines[index_line_begin].size();
      }
    }
    while ((font.getDirection() > 0) ? (offset_begin < bound_end) : (offset_begin > bound_end));

    font.clearClip();
  }
}
