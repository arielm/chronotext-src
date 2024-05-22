#pragma once

#include "chr/cross/Context.h"

#include "Episode.h"

class Sketch : public chr::CrossSketch
{
public:
  AudioManager audioManager;
  chr::xf::FontManager fontManager;

  Sketch();
  virtual ~Sketch() {}

  void setup() final;
  void shutdown() final;
  void resize() final;
  void update() final;
  void draw() final;

  void keyDown(int keyCode, int modifiers) final;

  void mouseMoved(float x, float y) final;
  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

  void prevEpisode(double rate);
  void nextEpisode(double rate);

protected:
  std::vector<std::shared_ptr<Episode>> episodes;
  std::shared_ptr<Episode> currentEpisode;
  int currentEpisodeIndex;
  bool currentEpisodeWasPlaying;

  bool mouseIsPressed = false;
  bool cursorVisible = true;
  chr::Clock::Ref cursorClock;

  void addEpisode(std::shared_ptr<Episode> episode);
  void setEpisode(int episodeIndex, double rate);

  void cursorChanged();
  void updateCursor();
};
