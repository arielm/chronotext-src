#include "TextHelix.h"

#include "chr/math/Utils.h"

using namespace std;
using namespace chr;
using namespace chr::gl;

void TextHelix::draw(XFont &font, const u16string &text, float r1, float r2, float h, float offsetX, float offsetY)
{
  float L = font.getStringAdvance(text);
  float turns = L / (PI * (r1 + r2));
  float l = TWO_PI * turns;
  float dz = h / l;
  float ay = -atan2f(h, L);
  
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
  float D = offsetX;
  
  Matrix matrix;
  
  for (auto c : text)
  {
    auto glyphIndex = font.getGlyphIndex(c);
    
    float half = 0.5f * font.getGlyphAdvance(glyphIndex);
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
        .setTranslate(-cosf(d) * r, +sinf(d) * r, d * dz)
        .rotateZ(HALF_PI - d)
        .rotateY(ay)
        .rotateX(-HALF_PI);
      
      font.addGlyph(matrix, glyphIndex, -half, offsetY);
    }
    
    D += half;
  }
}
