#include "Episode.h"

Episode::Episode(const Context &context, Lang lang)
:
clock(context.clock),
windowSize(context.windowSize),
fontManager(context.fontManager),
lang(lang)
{}

void Episode::performEnter()
{
  clock->restart();
  done = false;
  enter();
}

void Episode::nextEpisode()
{
  done = true;
}

bool Episode::isDone() const
{
  return done;
}
