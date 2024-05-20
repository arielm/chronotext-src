#include "TextNoisePatch.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;

TextNoisePatch::TextNoisePatch(NoiseSurface *surface, float polygonOffset, shared_ptr<XFont> font, shared_ptr<LineProvider> lineProvider, float lineHeight)
:
NoisePatch(surface),
polygonOffset(polygonOffset),
font(font),
lineProvider(lineProvider),
lineHeight(lineHeight)
{
  int capacity = int(surface->gridWidth / lineHeight);

  for (int i = 0; i < capacity; i++)
  {
    paths.emplace_back(surface->gridWidth, surface->gridSize);
  }
}

void TextNoisePatch::drawLine(const HPath &path, float ox, float oy, int lineIndex, float xm, float xp, const math::Rectf &clip)
{
  float total = lineProvider->linesLen[lineProvider->size];

  float bound_begin;
  float bound_end;
  float o_begin = 0;

  ox += font->getDirection() * lineProvider->linesLen[lineIndex];

  if (font->getDirection() > 0)
  {
    bound_begin = path.x2Pos(xm) + ox;
    bound_end = path.x2Pos(xp);

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
    bound_begin = path.x2Pos(xp) + ox;
    bound_end = path.x2Pos(xm);

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

  Matrix fontMatrix;
  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);

  int size = lineProvider->lines[index_line_begin].size();
  bool clipping = (clip.x1 != 0) || (clip.y1 != 0) || (clip.x2 != 0) || (clip.y2 != 0);

  do
  {
    int glyphIndex = font->getGlyphIndex((index_char_begin == size) ? u' ' : lineProvider->lines[index_line_begin][index_char_begin]);
    float half = 0.5f * font->getDirection() * font->getGlyphAdvance(glyphIndex);
    offset_begin += half;

    if (glyphIndex >= 0)
    {
      const HPath::Value value = path.pos2Value(offset_begin);

      fontMatrix
        .setTranslate(value.x, oy, value.z)
        .rotateXY(value.sx, value.sy);

      if (polygonOffset != 0)
      {
        fontMatrix.translate(0, 0, polygonOffset);
      }

      if (clipping)
      {
        font->setClip(clip - glm::vec2(value.x, oy));
        font->addGlyph(fontMatrix, glyphIndex, -half, baselineOffset);
      }
      else
      {
        font->addGlyph(fontMatrix, glyphIndex, -half, baselineOffset);
      }
    }

    offset_begin += half;

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

float TextNoisePatch::drawLine(int pathIndex, float xm, float xp, float ox, float oy, float d, const math::Rectf &clip)
{
  int j = math::bound(pathIndex, paths.size());
  HPath &path = paths[j];
  float modY = j * lineHeight + fmodf(oy, lineHeight);

  if (path.modY != modY || path.d != d)
  {
    compute(path, modY, d);
  }

  float y = pathIndex * lineHeight + fmodf(oy, lineHeight);
  drawLine(path, ox, y, math::bound(int(oy / lineHeight) - pathIndex, lineProvider->size), xm, xp, clip);

  return y;
}

void TextNoisePatch::drawText(float xm, float ym, float w, float h, float ox, float oy, float d)
{
  float xp = xm + w;
  float yp = ym + h;

  math::Rectf clip(glm::vec2(xm, ym), glm::vec2(xp, yp));

  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);
  float minus = ym + (baselineOffset + font->getAscent()) - fmodf(oy, lineHeight);
  float plus = yp - (baselineOffset - font->getDescent()) - fmodf(oy, lineHeight);

  int total_m = int(minus / lineHeight);
  int total_p = int(plus / lineHeight);

  for (int i = total_m; i <= total_p; i++)
  {
    drawLine(i, xm, xp, ox, oy, d, clip);
  }
}

void TextNoisePatch::compute(HPath &path, float modY, float d)
{
  path.modY = modY;
  path.d = d;

  float dx = path.segmentLength;
  float half = dx * 0.5f;
  float half2 = half * half;

  path.begin();

  for (float x = 0; x <= path.width; x += dx)
  {
    float z = d * surface->getHeight(x, modY);

    float dz_my = z - d * surface->getHeight(x, modY - half);
    float dz_py = d * surface->getHeight(x, modY + half) - z;
    float sin_ax = (dz_my / sqrtf(dz_my * dz_my + half2) + dz_py / sqrtf(dz_py * dz_py + half2)) * 0.5f;

    float dz_mx = d * surface->getHeight(x - half, modY) - z;
    float dz_px = z - d * surface->getHeight(x + half, modY);
    float sin_ay = (dz_mx / sqrtf(dz_mx * dz_mx + half2) + dz_px / sqrtf(dz_px * dz_px + half2)) * 0.5f;

    path.add(z, sin_ax, sin_ay);
  }

  path.end();
}
