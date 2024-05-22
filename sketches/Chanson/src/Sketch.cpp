#include "Sketch.h"
#include "Episode1.h"
#include "Episode2.h"
#include "Episode3.h"
#include "Episode4.h"

using namespace std;
using namespace chr;

Sketch::Sketch()
:
cursorClock(Clock::create())
{}

void Sketch::setup()
{
  audioManager.setup();

  addEpisode(make_shared<Episode1>(this));
  addEpisode(make_shared<Episode2>(this));
  addEpisode(make_shared<Episode3>(this));
  addEpisode(make_shared<Episode4>(this));

  setEpisode(0, +1);

  //

  cursorClock->start();

  // ---

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::shutdown()
{
  for (auto episode : episodes)
  {
    episode->shutdown();
  }

  audioManager.shutdown();
}

void Sketch::resize()
{
  for (auto episode : episodes)
  {
    episode->performResize(glm::ivec2(windowInfo.width, windowInfo.height));
  }
}

void Sketch::update()
{
  updateCursor();
  audioManager.update();
  currentEpisode->update();
}

void Sketch::draw()
{
  currentEpisode->draw();
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  switch (keyCode)
  {
    case KEY_UP:
    {
      audioManager.stop(); // XXX
      nextEpisode(+1);
      break;
    }

    case KEY_DOWN:
    {
      audioManager.stop(); // XXX
      prevEpisode(-1);
      break;
    }

    case KEY_SPACE:
    {
      currentEpisode->togglePlay();
      break;
    }

    case KEY_LEFT:
    {
      currentEpisode->setPlayingRate(-1);

      if (!currentEpisode->isPlaying())
      {
        currentEpisode->togglePlay();
      }

      break;
    }

    case KEY_RIGHT:
    {
      currentEpisode->setPlayingRate(+1);

      if (!currentEpisode->isPlaying())
      {
        currentEpisode->togglePlay();
      }

      break;
    }
  }
}

void Sketch::prevEpisode(double rate)
{
  int episodeIndex = currentEpisodeIndex - 1;

  if (episodeIndex < 0)
  {
    setEpisode(episodes.size() - 1, rate);
  }
  else
  {
    setEpisode(episodeIndex, rate);
  }
}

void Sketch::nextEpisode(double rate)
{
  int episodeIndex = currentEpisodeIndex + 1;

  if (episodeIndex >= episodes.size())
  {
    setEpisode(0, rate);
  }
  else
  {
    setEpisode(episodeIndex, rate);
  }
}

void Sketch::addEpisode(shared_ptr<Episode> episode)
{
  episodes.push_back(episode);
  episode->performSetup();
}

void Sketch::setEpisode(int episodeIndex, double rate)
{
  if (currentEpisode)
  {
    currentEpisode->stop(rate);
  }

  currentEpisodeIndex = episodeIndex;
  currentEpisode = episodes[currentEpisodeIndex];

  currentEpisode->start(rate);
  currentEpisode->update(); // NECESSARY, BECAUSE THE NEW EPISODE'S draw() IS ABOUT TO BE CALLED
}

void Sketch::mouseMoved(float x, float y)
{
  cursorChanged();
}

void Sketch::mouseDragged(int button, float x, float y)
{
  cursorChanged();
}

void Sketch::mousePressed(int button, float x, float y)
{
  mouseIsPressed = true;
  cursorChanged();
}

void Sketch::mouseReleased(int button, float x, float y)
{
  mouseIsPressed = false;
  cursorChanged();
}

void Sketch::cursorChanged()
{
  if (!cursorVisible)
  {
    delegate().showCursor();
    cursorVisible = true;
  }

  cursorClock->restart();
}

void Sketch::updateCursor()
{
  if (cursorVisible && !mouseIsPressed)
  {
    if (cursorClock->getTime() > 3)
    {
      delegate().hideCursor();
      cursorVisible = false;
    }
  }
}
