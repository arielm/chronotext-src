#pragma once

#include "Button.h"

class TextButton : public Button
{
public:
  TextButton() = default;
  TextButton(std::shared_ptr<UI> ui, const std::u16string &text);

  void draw() override;

protected:
  std::u16string text;
};
