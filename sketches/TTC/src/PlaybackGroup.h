#pragma once

#include "Group.h"
#include "Tree.h"
#include "Screen.h"
#include "ImageButton.h"
#include "TextButton.h"
#include "ImageItem.h"
#include "ClockItem.h"

class PlaybackGroup : public Group
{
public:
  ImageButton buttonRewind, buttonPlay, buttonPause;
  ClockItem itemClock;
  ImageItem itemCross;
  TextButton buttonSpeed1, buttonSpeed4, buttonSpeed16;

  PlaybackGroup(std::shared_ptr<UI> ui);

  void resize() final;
  void draw() final;

  void enter(Screen *screen, Tree *tree);
  void run(Tree *tree);
  void event(Tree *tree, EventCaster *source, int message);

protected:
  void setClockRate(chr::Clock &clock, float rate);
};
