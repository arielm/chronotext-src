#pragma once

#include "chr/InputSource.h"

#include "common/xf/Font.h"

class LineProvider
{
public:
  std::vector<std::u16string> lines;
  int size;
  
  std::vector<float> linesLen;
  std::vector<std::vector<float>> charsLen;
  
  LineProvider() = default;
  LineProvider(chr::XFont &font, float fontSize, const chr::InputSource &inputSource);

  static std::shared_ptr<LineProvider> create(chr::XFont &font, float fontSize, const chr::InputSource &inputSource)
  {
    return std::shared_ptr<LineProvider>(new LineProvider(font, fontSize, inputSource));
  }
};
