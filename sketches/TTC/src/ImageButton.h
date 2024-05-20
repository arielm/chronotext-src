#pragma once

#include "Button.h"

class ImageButton : public Button
{
public:
  ImageButton() = default;
  ImageButton(std::shared_ptr<UI> ui, const std::string &name);

  void draw() override;

protected:
  std::string name;
};
