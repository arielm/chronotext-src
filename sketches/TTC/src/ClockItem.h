#pragma once

#include "chr/math/Rect.h"

#include <memory>

class UI;

class ClockItem
{
public:
  ClockItem() = default;
  ClockItem(std::shared_ptr<UI> ui);

  float getWidth();
  void setBounds(const chr::math::Rectf &bounds);
  void setMediaTime(int time);
  void draw();

protected:
  std::shared_ptr<UI> ui;
  chr::math::Rectf bounds;
  int time;
};
