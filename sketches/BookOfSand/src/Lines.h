#pragma once

#include "common/xf/Font.h"

class Lines
{
public:
  std::u16string text;
  std::vector<int> indices;
  std::vector<int> lengths;
  int size = 0;

  void setText(std::shared_ptr<chr::XFont> font, const std::u16string &text);
  void wrap(float width);
  float getLineWidth(int index);

protected:
  std::shared_ptr<chr::XFont> font;

  void addLine(int index, int length);
};
