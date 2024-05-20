#include "TextRectangle.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;

TextRectangle::TextRectangle(shared_ptr<XFont> font, shared_ptr<LineProvider> lineProvider, float lineHeight)
:
font(font),
lineProvider(lineProvider),
lineHeight(lineHeight)
{}

void TextRectangle::drawLine(float ox, float oy, int lineIndex, float xm, float xp)
{
  float total = lineProvider->linesLen[lineProvider->size];

  float bound_begin;
  float bound_end;
  float o_begin = 0;

  ox += font->getDirection() * lineProvider->linesLen[lineIndex];

  if (font->getDirection() > 0)
  {
    bound_begin = xm + ox;
    bound_end = xp;

    if (bound_begin < 0)
    {
      o_begin = total * int(bound_begin / total) - total;
      bound_begin = math::boundf(bound_begin, total);
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

  auto index_line_begin = utils::search(lineProvider->linesLen, bound_begin, 1, lineProvider->linesLen.size());
  auto index_char_begin = utils::search(lineProvider->charsLen[index_line_begin], bound_begin, 1, lineProvider->charsLen[index_line_begin].size());
  float offset_begin = font->getDirection() * (o_begin + lineProvider->charsLen[index_line_begin][index_char_begin]) - ox;

  // ---

  int size = lineProvider->lines[index_line_begin].size();

  do
  {
    int glyphIndex = font->getGlyphIndex((index_char_begin == size) ? u' ' : lineProvider->lines[index_line_begin][index_char_begin]);
    font->addGlyph(matrix, glyphIndex, offset_begin, oy);
    offset_begin += font->getDirection() * font->getGlyphAdvance(glyphIndex);

    if (++index_char_begin > size)
    {
      index_char_begin = 0;
      index_line_begin = (index_line_begin + 1) % lineProvider->size;

      size = lineProvider->lines[index_line_begin].size();
    }
  }
  while ((font->getDirection() > 0) ? (offset_begin < bound_end) : (offset_begin > bound_end));
}

void TextRectangle::drawText(float xm, float ym, float w, float h, float ox, float oy)
{
  float xp = xm + w;
  float yp = ym + h;

  font->setClip(xm, ym, xp, yp);

  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);
  float minus = ym + (baselineOffset + font->getAscent()) - fmodf(oy, lineHeight);
  float plus = yp - (baselineOffset - font->getDescent()) - fmodf(oy, lineHeight);

  int total_m = int(minus / lineHeight);
  int total_p = int(plus / lineHeight);

  // ---

  for (int i = total_m; i <= total_p; i++)
  {
    drawLine(ox, i * lineHeight + fmodf(oy, lineHeight) + baselineOffset, math::bound(int(oy / lineHeight) - i, lineProvider->size), xm, xp);
  }

  font->clearClip();
}

void TextRectangle::update(float x, float y, float w, float h, float gridSize)
{
  /*
   * FILL
   */

  fillBatch.clear();

  fillBatch.addVertices(
    matrix.transformPoint(x, y),
    matrix.transformPoint(x, y + h),
    matrix.transformPoint(x + w, y + h),
    matrix.transformPoint(x + w, y));

  /*
   * GRID
   */

  float ox1 = floorf(x / gridSize);
  float ox2 = ceilf((x + w) / gridSize);

  float oy1 = floorf(y / gridSize);
  float oy2 = ceilf((y + h) / gridSize);

  int nx = int(ox2 - ox1);
  int ny = int(oy2 - oy1);

  float x1 = gridSize * ox1;
  float x2 = x1 + gridSize * nx;

  float y1 = gridSize * oy1;
  float y2 = y1 + gridSize * ny;

  // ---

  gridBatch.clear();
  gridBatch.vertexBuffer()->storage.reserve((nx + ny + 2) * 2);

  for (int iy = 0; iy <= ny; iy++)
  {
    float yy = y1 + gridSize * iy;

    gridBatch.addVertices(
      matrix.transformPoint(x1, yy),
      matrix.transformPoint(x2, yy));
  }

  for (int ix = 0; ix <= nx; ix++)
  {
    float xx = x1 + gridSize * ix;

    gridBatch.addVertices(
      matrix.transformPoint(xx, y1),
      matrix.transformPoint(xx, y2));
  }
}

void TextRectangle::drawFill(const glm::vec4 &color)
{
  fillBatch
    .setPrimitive(GL_TRIANGLE_FAN)
    .setShaderColor(color)
    .flush();
}

void TextRectangle::drawGrid(const glm::vec4 &color)
{
  gridBatch
    .setPrimitive(GL_LINES)
    .setShaderColor(color)
    .flush();
}
