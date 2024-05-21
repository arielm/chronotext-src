#include "SpiralPath.h"

#include "chr/math/Utils.h"

using namespace chr;

SpiralPath::SpiralPath(float r1, float r2, float turns, float dir)
:
r1(r1),
r2(r2),
turns(turns),
dir(dir)
{}

SpiralPath::Value SpiralPath::offsetToValue(float offset) const
{
  Value value;

  float l = TWO_PI * turns;
  float dr = (r2 - r1) / l;

  float r = sqrtf(r1 * r1 + 2 * dr * offset);
  float d = (r - r1) / dr;

  float sx = +dir * sinf(dir * d);
  float cy = -dir * cosf(dir * d);

  value.x = sx * r;
  value.y = cy * r;
  value.az = dir * d;

  return value;
}

SpiralPath::Value SpiralPath::offsetToValue(float offset, float d1, float d2) const
{
  Value value;

  float l = TWO_PI * turns;
  float dr = (r2 - r1) / l;

  float r = sqrtf(r1 * r1 + 2 * dr * offset);
  float d = (r - r1) / dr;

  float sx = +dir * sinf(dir * d);
  float cy = -dir * cosf(dir * d);

  float rd1 = r + d1;
  value.quad.x1 = sx * rd1;
  value.quad.y1 = cy * rd1;

  float rd2 = r + d2;
  value.quad.x2 = sx * rd2;
  value.quad.y2 = cy * rd2;

  return value;
}
