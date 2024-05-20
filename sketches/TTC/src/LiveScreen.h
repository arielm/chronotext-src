#pragma once

#include "Screen.h"
#include "Tree.h"
#include "ImageButton.h"
#include "ClockItem.h"

class LiveScreen : public Screen
{
public:
  LiveScreen(std::shared_ptr<UI> ui);

  void setCurrentTree(Tree *tree);

  void resize() final;
  void run() final;
  void draw() final;
  void enter() final;

  void event(EventCaster *source, int message) final;

protected:
  Tree *currentTree = nullptr;

  ImageButton buttonRecord, buttonStop;
  ClockItem itemClock;
};
