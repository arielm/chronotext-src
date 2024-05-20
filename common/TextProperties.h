#pragma once

#include "common/LineProvider.h"

namespace chr
{
  struct TextProperties
  {
    glm::vec4 color;
    std::shared_ptr<XFont> font;
    std::shared_ptr<LineProvider> lineProvider;
    float fontSize;
    float lineHeight;
  };
}
