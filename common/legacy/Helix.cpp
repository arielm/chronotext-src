#include "Helix.h"

using namespace std;
using namespace chr;

Helix::Helix(std::shared_ptr<chr::xf::Font> font)
:
font(font)
{}

void Helix::update(float r1, float r2, float turns, float h, float DD1, float DD2)
{
  wireBatch.clear();

  float l = TWO_PI * turns;
  float L = PI * turns * (r1 + r2);
  float dz = h / l;
  float DD = (DD2 - DD1) / l;

  float r;
  float dr;
  bool conical = (fabsf(r1 - r2) > 0.00001f); // AVOIDS INFINITY AND DIVISIONS-BY-ZERO WITH CYLINDRICAL HELICES (r1 = r2)

  if (conical)
  {
    dr = (r2 - r1) / l;
  }
  else
  {
    r = r1;
  }

  float d;
  float D = 0;
  float direction = font->getDirection();

  do
  {
    if (conical)
    {
      r = sqrtf(r1 * r1 + 2 * dr * D);
      d = (r - r1) / dr;
    }
    else
    {
      d = D / r;
    }

    D += DD1 + d * DD;
    wireBatch.addVertex(-cosf(-direction * d) * r, +sinf(-direction * d) * r, d * dz);
  }
  while (D < L);
}

void Helix::drawText(const u16string &text, float r1, float r2, float turns, float h, float D, float size1, float size2)
{
  float direction = font->getDirection();

  float l = TWO_PI * turns;
  float L = PI * turns * (r1 + r2);
  float dz = h / l;
  float ay = -direction * atan2f(h, L);

  float r;
  float dr;
  float ax;
  bool conical = (fabsf(r1 - r2) > 0.00001f); // AVOIDS INFINITY AND DIVISIONS-BY-ZERO WITH CYLINDRICAL HELICES (r1 = r2)

  if (conical)
  {
    dr = (r2 - r1) / l;
    ax = -HALF_PI - atan2f(r2 - r1, h);
  }
  else
  {
    r = r1;
    ax = -HALF_PI;
  }

  float d;
  int size = text.size();
  int index = 0;

  Matrix matrix;

  do
  {
    font->setSize(size1 + (D / L) * (size2 - size1));
    float baselineOffset = font->getOffsetY(XFont::ALIGN_MIDDLE);

    wchar_t c = (index == size) ? u' ' : text[index];
    int glyphIndex = font->getGlyphIndex(c);

    if (++index > size)
    {
      index = 0;
    }

    float half = 0.5f * font->getGlyphAdvance(glyphIndex);
    D += half;

    if (glyphIndex >= 0)
    {
      if (conical)
      {
        r = sqrtf(r1 * r1 + 2 * dr * D);
        d = (r - r1) / dr;
      }
      else
      {
        d = D / r;
      }

      matrix
        .setTranslate(-cosf(direction * d) * r, +sinf(direction * d) * r, d * dz)
        .rotateZ(HALF_PI - direction * d)
        .rotateY(ay)
        .rotateX(ax);

      font->addGlyph(matrix, glyphIndex, -half, baselineOffset);
    }

    D += half;
  }
  while (D < L);
}
