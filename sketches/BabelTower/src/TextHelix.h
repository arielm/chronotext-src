#pragma once

#include "common/xf/Font.h"

class TextHelix
{
public:
  static void draw(chr::XFont &font, const std::u16string &text, float r1, float r2, float h, float offsetX = 0, float offsetY = 0);
};
