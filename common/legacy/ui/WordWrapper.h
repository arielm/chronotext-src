#pragma once

#include "common/xf/Font.h"

namespace chr
{
  class WordWrapper
  {
  public:
    size_t size;
    std::vector<int> offsets;
    std::vector<int> lengths;

    WordWrapper() = default;

    float wrap(const XFont &font, const std::u16string &text);
    void wrap(const XFont &font, const std::u16string &text, float width);
    
    int getLine(int offset);

  protected:
    void addLine(const std::u16string &text, int offset, int length);
  };
}
