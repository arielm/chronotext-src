#include "Sketch.h"

#include "Episode1A.h"
#include "Episode1B.h"
#include "Episode2A.h"
#include "Episode2B.h"
#include "Episode3A.h"
#include "Episode3B.h"

using namespace std;
using namespace chr;
using namespace gl;

const float minAspectRatio = 16 / 9.0f;
const float maxAspectRatio = 16 / 9.0f;
const float aspectRatio = 16 / 9.0f;

Sketch::Sketch()
:
cursorClock(Clock::create())
{}

void Sketch::setup()
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // ---

  Episode::Context episodeContext(clock(), windowInfo.size, fontManager);

  episodes.push_back(make_shared<Episode1A>(episodeContext, Episode::LANG_HE));
  episodes.push_back(make_shared<Episode1B>(episodeContext, Episode::LANG_HE));
  episodes.push_back(make_shared<Episode2A>(episodeContext, Episode::LANG_HE));
  episodes.push_back(make_shared<Episode2B>(episodeContext, Episode::LANG_HE));
  episodes.push_back(make_shared<Episode3A>(episodeContext, Episode::LANG_HE));
  episodes.push_back(make_shared<Episode3B>(episodeContext, Episode::LANG_HE));

  episodes.push_back(make_shared<Episode1A>(episodeContext, Episode::LANG_EN));
  episodes.push_back(make_shared<Episode1B>(episodeContext, Episode::LANG_EN));
  episodes.push_back(make_shared<Episode2A>(episodeContext, Episode::LANG_EN));
  episodes.push_back(make_shared<Episode2B>(episodeContext, Episode::LANG_EN));
  episodes.push_back(make_shared<Episode3A>(episodeContext, Episode::LANG_EN));
  episodes.push_back(make_shared<Episode3B>(episodeContext, Episode::LANG_EN));

  for (auto episode : episodes)
  {
    episode->setup();
  }

  // ---

  currentEpisode = episodes[currentEpisodeIndex];
  currentEpisode->performEnter();

  cursorClock->start();
}

void Sketch::resize()
{
  int windowWidth = windowInfo.width;
  int windowHeight = windowInfo.height;
  float windowAspectRatio = windowInfo.aspectRatio();

  int viewportWidth = windowWidth;
  int viewportHeight = windowHeight;

  if ((windowAspectRatio < minAspectRatio) || (windowAspectRatio > maxAspectRatio))
  {
    if (windowAspectRatio < aspectRatio)
    {
      viewportHeight = int(windowWidth / aspectRatio);
    }
    else
    {
      viewportWidth = int(windowHeight * aspectRatio);
    }
  }

  int viewportX = (int)ceilf((windowWidth - viewportWidth) * 0.5f);
  int viewportY = (int)ceilf((windowHeight - viewportHeight) * 0.5f);

  viewport = math::Recti(viewportX, viewportY, viewportWidth, viewportHeight);
}

void Sketch::update()
{
  updateCursor();
  currentEpisode->update();

  if (currentEpisode->isDone())
  {
    nextEpisode();
    currentEpisode->update();
  }
}

void Sketch::draw()
{
  float contentScale = getDisplayInfo().contentScale;

  glViewport(viewport.x1 * contentScale, viewport.y1 * contentScale, viewport.width() * contentScale, viewport.height() * contentScale);

  currentEpisode->draw();

  glm::ivec2 viewportUL = viewport.x1y1();
  glm::ivec2 windowSize = windowInfo.size;

  glClearColor(0, 0, 0, 1);
  glEnable(GL_SCISSOR_TEST);

  glScissor(0, 0, viewportUL.x * contentScale, windowSize.y * contentScale);
  glClear(GL_COLOR_BUFFER_BIT);

  glScissor(0, 0, windowSize.x * contentScale, viewportUL.y * contentScale);
  glClear(GL_COLOR_BUFFER_BIT);

  glScissor((windowSize.x - viewportUL.x) * contentScale, 0, viewportUL.x * contentScale, windowSize.y * contentScale);
  glClear(GL_COLOR_BUFFER_BIT);

  glScissor(0, (windowSize.y - viewportUL.y) * contentScale, windowSize.x * contentScale, viewportUL.y * contentScale);
  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_SCISSOR_TEST);
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  switch (keyCode)
  {
    case KEY_SPACE:
      togglePause();
      break;

    case KEY_UP:
      nextEpisode();
      break;
  }
}

void Sketch::nextEpisode()
{
  currentEpisode->leave();

  currentEpisodeIndex = (currentEpisodeIndex + 1) % episodes.size();
  currentEpisode = episodes[currentEpisodeIndex];
  currentEpisode->performEnter();
}

void Sketch::togglePause()
{
  if (paused)
  {
    clock()->start();
  }
  else
  {
    clock()->stop();
  }

  paused ^= true;
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
