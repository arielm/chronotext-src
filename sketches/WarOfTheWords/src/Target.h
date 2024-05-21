#pragma once

#include <memory>

class TextSpan;

struct Target
{
  std::shared_ptr<TextSpan> span;
  float x, y, z;

  Target() = default;

  Target(std::shared_ptr<TextSpan> span, float x, float y)
  :
  span(span),
  x(x),
  y(y),
  z(0)
  {}
};
