#pragma once

#include "Quad.h"

class SpiralPath
{
public:
  struct Value
  {
    float x, y;
    float az;
    twitter::Quad quad;
  };

  SpiralPath() = default;
  SpiralPath(float r1, float r2, float turns, float dir);

  SpiralPath::Value offsetToValue(float offset) const;
  SpiralPath::Value offsetToValue(float offset, float d1, float d2) const;

protected:
  float r1;
  float r2;
  float turns;
  float dir;
};
