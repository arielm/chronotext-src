#pragma once

#include "Episode.h"
#include "SeaBrickSpiral.h"
#include "Run.h"

#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/time/Clock.h"
#include "chr/Random.h"

class Episode4 : public Episode
{
public:
  Episode4(Sketch *sketch);

  void setup() final;
  void update() final;
  void draw() final;

  void start(double rate) final;
  void stop(double rate) final;

  void pause() final;
  void resume() final;

  bool isPlaying() final;
  void togglePlay() final;
  void setPlayingRate(double rate) final;

protected:
  chr::xf::FontSequence fontSequence;

  chr::Clock::Ref clock;
  chr::Clock::Ref surfaceClock;

  bool playing;
  double playingRate;

  double startTime;
  double endTime;

  SeaSurface surface;
  SeaSurfaceSpiral2 spiral;
  SeaBrickSpiral brickSpiral;

  chr::gl::IndexedVertexBatch<> capBatch;
  chr::gl::IndexedVertexBatch<> wallBatch;
  chr::gl::IndexedVertexBatch<> strokeBatch;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  float brickHeight;
  float brickDepth;
  float brickPadding;
  float brickMargin;

  float spiralLength;
  float visualToSoundFactor;

  std::vector<Run> runs;
  bool randomizeRuns;
  chr::Random rand;

  float setupBricks(float startOffset, float endOffset);
  void updateBricks();
  std::vector<Brick1>::iterator findBrick(unsigned int offset);
  std::vector<Brick1>::iterator findBrick(int stanzaIndex, int verseIndex, int wordIndex);
};
