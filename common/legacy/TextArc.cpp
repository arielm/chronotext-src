#include "TextArc.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;

TextArc::TextArc(shared_ptr<XFont> font, shared_ptr<LineProvider> lineProvider, float lineHeight)
:
font(font),
lineProvider(lineProvider),
lineHeight(lineHeight)
{}

void TextArc::drawLine(float ox, float oy, float r, int lineIndex, float xm, float xp, const math::Rectf &clip)
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

  float y = sinf(oy / r) * r;
  float z = cosf(oy / r) * r;

  auto fontMatrix = matrix;
  fontMatrix
    .translate(0, y, -z)
    .rotateX(oy / r);

  font->setClip(clip - glm::vec2(0, oy));

  int size = lineProvider->lines[index_line_begin].size();

  do
  {
    int glyphIndex = font->getGlyphIndex((index_char_begin == size) ? u' ' : lineProvider->lines[index_line_begin][index_char_begin]);
    font->addGlyph(fontMatrix, glyphIndex, offset_begin, 0);
    offset_begin += font->getDirection() * font->getGlyphAdvance(glyphIndex);

    if (++index_char_begin > size)
    {
      index_char_begin = 0;
      index_line_begin = (index_line_begin + 1) % lineProvider->size;

      size = lineProvider->lines[index_line_begin].size();
    }
  }
  while ((font->getDirection() > 0) ? (offset_begin < bound_end) : (offset_begin > bound_end));

  font->clearClip();
}

void TextArc::drawText(float xm, float ym, float w, float r, float a, float ox, float oy)
{
  float xp = xm + w;
  float yp = ym + a * r;

  math::Rectf clip(glm::vec2(xm, ym), glm::vec2(xp, yp));

  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);
  float minus = ym + (baselineOffset + font->getAscent()) - fmodf(oy, lineHeight);
  float plus = yp - (baselineOffset - font->getDescent()) - fmodf(oy, lineHeight);

  int total_m = int(minus / lineHeight);
  int total_p = int(plus / lineHeight);

  // ---

  for (int i = total_m; i <= total_p; i++)
  {
    drawLine(ox, i * lineHeight + fmodf(oy, lineHeight) + baselineOffset, r, math::bound(int(oy / lineHeight) - i, lineProvider->size), xm, xp, clip);
  }
}

void TextArc::update(float x, float y, float w, float r, float a, float gridSize, float dd)
{
  /*
   * FILL
   */

  float ox1 = floorf(x / dd);
  float ox2 = ceilf((x + w) / dd);

  int nx = int(ox2 - ox1);
  int na = (int)ceilf(a * r / dd) + 1;

  float x1 = dd * ox1;
  float x2 = x1 + dd * nx;

  // ---

  fillBatch.clear();
  fillBatch.vertexBuffer()->storage.reserve(na * 2);

  for (int ia = 0; ia < na; ia++)
  {
    float d = ia * dd / r;
    float yy = sinf(d) * r;
    float zz = cosf(d) * r;

    fillBatch.addVertices(
      matrix.transformPoint(x1, yy, zz),
      matrix.transformPoint(x2, yy, zz));
  }

  /*
   * GRID
   */

  ox1 = floorf(x / gridSize);
  ox2 = ceilf((x + w) / gridSize);

  nx = int(ox2 - ox1);
  na = (int)ceilf(a * r / gridSize) + 1;

  x1 = gridSize * ox1;
  x2 = x1 + gridSize * nx;

  // ---

  vector<float> yy;
  vector<float> zz;

  yy.reserve(na);
  zz.reserve(na);

  for (int ia = 0; ia < na; ia++)
  {
    float d = ia * gridSize / r;
    yy.push_back(sinf(d) * r);
    zz.push_back(cosf(d) * r);
  }

  // ---

  gridBatch.clear();
  gridBatch.vertexBuffer()->storage.reserve(na * 2 + (nx + 1) * (na - 1) * 2);

  /*
   * LATERAL LINES
   */
  for (int ia = 0; ia < na; ia++)
  {
    gridBatch.addVertices(
      matrix.transformPoint(x1, yy[ia], zz[ia]),
      matrix.transformPoint(x2, yy[ia], zz[ia]));
  }

  /*
   * LONGITUDINAL LINES
   */
  for (int ix = 0; ix <= nx; ix++)
  {
    for (int ia = 0; ia < na - 1; ia++)
    {
      float xx = x1 + gridSize * ix;

      gridBatch.addVertices(
        matrix.transformPoint(xx, yy[ia], zz[ia]),
        matrix.transformPoint(xx, yy[ia + 1], zz[ia + 1]));
    }
  }
}

void TextArc::drawFill(const glm::vec4 &color)
{
  fillBatch
    .setPrimitive(GL_TRIANGLE_STRIP)
    .setShaderColor(color)
    .flush();
}

void TextArc::drawGrid(const glm::vec4 &color)
{
  gridBatch
    .setPrimitive(GL_LINES)
    .setShaderColor(color)
    .flush();
}
