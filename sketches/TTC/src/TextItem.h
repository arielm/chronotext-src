#pragma once

#include "common/xf/Font.h"

class UI;

class TextItem
{
public:
  TextItem() = default;
  TextItem(std::shared_ptr<UI> ui, const std::u16string &text, chr::XFont::Alignment alignX);

  float getWidth();
  void setPosition(const glm::vec2 &position);
  void draw();

protected:
  std::shared_ptr<UI> ui;
  std::u16string text;
  chr::XFont::Alignment alignX;
  glm::vec2 position;
};
