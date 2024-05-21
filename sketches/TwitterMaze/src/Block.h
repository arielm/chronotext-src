#pragma once

#include "Quad.h"

#include <vector>

struct Block
{
  size_t size;
  std::vector<twitter::Quad> quad;
  float h;

  Block() = default;

  Block(size_t size, float h)
  :
  size(size),
  h(h)
  {
    quad.resize(size + 1);
  }
};
