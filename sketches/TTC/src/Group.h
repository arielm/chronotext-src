#pragma once

#include <memory>

class UI;

class Group
{
public:
  Group(std::shared_ptr<UI> ui)
  :
  ui(ui)
  {}

  virtual ~Group() {};

  virtual void resize() {}
  virtual void draw() {}

protected:
  std::shared_ptr<UI> ui;
};
