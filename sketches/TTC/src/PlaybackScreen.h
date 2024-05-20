#pragma once

#include "Screen.h"
#include "Tree.h"

class PlaybackScreen : public Screen
{
public:
  PlaybackScreen(std::shared_ptr<UI> ui);

  void setCurrentTree(Tree *tree);

  void resize() final;
  void run() final;
  void draw() final;
  void enter() final;

  void event(EventCaster *source, int message) final;

protected:
  Tree *currentTree = nullptr;
};
