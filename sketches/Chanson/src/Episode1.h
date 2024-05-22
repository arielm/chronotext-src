#pragma once

#include "Episode.h"
#include "SeaBrickSpiral.h"

#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/time/Clock.h"

class Episode1 : public Episode
{
public:
  Episode1(Sketch *sketch);

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
  glm::quat cameraOrientation;
  glm::vec3 cameraTarget;

  chr::xf::FontSequence fontSequence;

  chr::Clock::Ref surfaceClock;
  SeaSurface surface;
  SeaSurfaceSpiral2 spiral;
  SeaBrickSpiral brickSpiral;

  unsigned int startOffset;
  unsigned int endOffset;

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

  float setupBricks(float startOffset, float endOffset);

  std::vector<Brick1>::iterator findBrick(unsigned int offset);
  std::vector<Brick1>::iterator findBrick(int stanzaIndex, int verseIndex, int wordIndex);
};
