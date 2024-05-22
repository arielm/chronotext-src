#pragma once

#include "Stanza.h"
#include "Word.h"
#include "AudioManager.h"

#include "common/xf/FontManager.h"

class Sketch;

class Episode
{
public:
  Episode(Sketch *sketch);
  virtual ~Episode() {}

  void performSetup();
  void performResize(const glm::ivec2 &size);

  virtual void setup() {}
  virtual void shutdown() {}
  virtual void resize(const glm::ivec2 &size) {}

  virtual void update() {}
  virtual void draw() {}

  virtual void start(double rate) {}
  virtual void stop(double rate) {}

  virtual void pause() {}
  virtual void resume() {}

  virtual bool isPlaying() = 0;
  virtual void togglePlay() = 0;
  virtual void setPlayingRate(double rate) = 0;

protected:
  Sketch *sketch;
  glm::ivec2 screenSize;
  AudioManager &audioManager;
  chr::xf::FontManager &fontManager;
  std::shared_ptr<chr::XFont> font;
  std::vector<Word> words;

  float setupWords();
  void loadStanza(const chr::InputSource &source, int stanzaIndex, const Stanza::FirstIndex &firstIndex = Stanza::FirstIndex(), const Stanza::LastIndex &lastIndex = Stanza::LastIndex());
};
