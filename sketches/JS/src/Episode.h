#pragma once

#include "chr/time/Clock.h"

#include "common/xf/FontManager.h"

class Episode
{
public:
  enum Lang
  {
    LANG_HE,
    LANG_EN,
    LANG_FR
  };

  struct Context
  {
    chr::Clock::Ref clock;
    glm::vec2 windowSize;
    chr::xf::FontManager &fontManager;

    Context(chr::Clock::Ref clock, const glm::vec2 &windowSize, chr::xf::FontManager &fontManager)
    :
    clock(clock),
    windowSize(windowSize),
    fontManager(fontManager)
    {}
  };

  Episode(const Context &context, Lang lang);

  virtual void setup() {}
  virtual void enter() {}
  virtual void leave() {}
  virtual void update() {}
  virtual void draw() {}

  void performEnter();
  bool isDone() const;

protected:
  chr::Clock::Ref clock;
  glm::vec2 windowSize;
  chr::xf::FontManager &fontManager;
  Lang lang;

  bool done;

  void nextEpisode();
};
