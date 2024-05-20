#pragma once

#include "chr/math/Rect.h"

#include <vector>
#include <memory>

class Tree;
class Branch;
class UI;

class Navigator
{
public:
  void reset();
  void mouseIn(int t);
  void codeIn(int code, int t);
  void run();
  void resize();
  void draw();

  Navigator(Tree *tree, std::shared_ptr<UI> ui);

protected:
  Tree *tree = nullptr;
  std::shared_ptr<UI> ui;
  std::vector<chr::math::Rectf> boxes;
  std::vector<std::shared_ptr<Branch>> slot_branches;
  int slot_active_n;
  int over = - 1;
  bool armed = false;
};
