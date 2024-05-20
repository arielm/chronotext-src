#pragma once

#include "chr/math/Rect.h"

#include <memory>

class UI;

class ImageItem
{
public:
  ImageItem() = default;
  ImageItem(std::shared_ptr<UI> ui, const std::string &name);

  void setBounds(const chr::math::Rectf &bounds);
  void draw();

protected:
  std::shared_ptr<UI> ui;
  chr::math::Rectf bounds;
  std::string name;
};
