#pragma once

#include "EventListener.h"

#include <memory>

class UI;

class Screen : public EventListener
{
public:
  Screen(std::shared_ptr<UI> ui)
  :
  ui(ui)
  {}

  ~Screen() {}

  virtual void resize() {}
  virtual void run() {}
  virtual void draw() {}
  virtual void enter() {}

protected:
  std::shared_ptr<UI> ui;
};
