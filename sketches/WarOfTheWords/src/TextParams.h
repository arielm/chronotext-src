#pragma

#include "common/xf/Font.h"

struct TextParams
{
  float marginLeft;
  float marginTop;
  float marginRight;
  float marginBottom;

  float paddingLeft;
  float paddingTop;
  float paddingRight;
  float paddingBottom;

  std::string fontFamily;
  float fontSize;
  glm::vec4 fontColor = { 0, 0, 0, 1 };

  chr::XFont::Alignment textAlignX;
  chr::XFont::Alignment textAlignY;

  bool wrap;

  float offsetX;
  float offsetY;

  float lineHeight;
  float lineHeightFactor;
};
