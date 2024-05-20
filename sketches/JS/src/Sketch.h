#pragma once

#include "chr/cross/Context.h"

#include "Episode.h"

class Sketch : public chr::CrossSketch
{
public:
  Sketch();
  virtual ~Sketch() {}

  void setup() final;
  void resize() final;
  void update() final;
  void draw() final;

  void keyDown(int keyCode, int modifiers) final;

  void mouseMoved(float x, float y) final;
  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

protected:
  chr::xf::FontManager fontManager;

  std::vector<std::shared_ptr<Episode>> episodes;
  std::shared_ptr<Episode> currentEpisode;
  int currentEpisodeIndex = 0;
  bool paused = false;

  bool mouseIsPressed = false;
  bool cursorVisible = true;
  chr::Clock::Ref cursorClock;

  chr::math::Recti viewport;

  void nextEpisode();
  void togglePause();

  void cursorChanged();
  void updateCursor();
};
