#pragma once

#include "Episode.h"
#include "WaveBrickLine.h"
#include "ThinWaveBrickLine.h"
#include "ThinFlatBrickLine.h"

#include "chr/gl/Batch.h"
#include "chr/time/Clock.h"

class Episode3 : public Episode
{
public:
  Episode3(Sketch *sketch);

  void setup() final;
  void shutdown() final;
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
  chr::Clock::Ref surfaceClock;

  unsigned int startOffset;
  unsigned int endOffset;

  float scrollingOffset;
  bool scrollingExtra;

  WaveSurface *wave;
  std::vector<WaveSurfaceLine*> waveLines;

  float brickHeight;
  float brickDepth;
  float brickPadding;
  float brickMargin;
  float segmentLength;

  float cornerPosition;
  float thinFlatBricksFarClip;
  float waveBricksFarClip;

  float lineLength;
  float visualToSoundFactor;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;

  chr::gl::IndexedVertexBatch<> waveBrickCapBatch;
  chr::gl::IndexedVertexBatch<> waveBrickWallBatch;
  chr::gl::IndexedVertexBatch<> waveBrickStrokeBatch;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> waveBrickVertexBuffer;
  chr::gl::Buffer<GLuint> waveBrickCapIndexBuffer;
  chr::gl::Buffer<GLuint> waveBrickWallIndexBuffer;
  chr::gl::Buffer<GLuint> waveBrickStrokeIndexBuffer;

  chr::gl::IndexedVertexBatch<> thinWaveBrickStrokeBatch;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> thinWaveBrickVertexBuffer;
  chr::gl::Buffer<GLuint> thinWaveBrickStrokeIndexBuffer;

  chr::gl::IndexedVertexBatch<> thinFlatBrickStrokeBatch;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> thinFlatBrickVertexBuffer;
  chr::gl::Buffer<GLuint> thinFlatBrickStrokeIndexBuffer;

  chr::xf::FontSequence waveFontSequence;
  chr::xf::FontSequence thinWaveFontSequence;
  chr::xf::FontSequence thinFlatFontSequence;

  std::vector<WaveBrickLine> waveBrickLines;
  std::vector<ThinWaveBrickLine> thinWaveBrickLines;
  std::vector<ThinFlatBrickLine> thinFlatBrickLines;

  WaveBrickLine *referenceBrickLine;
  std::vector<Brick3>::const_iterator referenceBrick;

  void setupLines(float sampleSize);
  float setupBricks(float startOffset, float endOffset);
  void updateBricks();
  std::vector<Brick3>::iterator findBrick(unsigned int offset);
  std::vector<Brick3>::const_iterator findBrick(int stanzaIndex, int verseIndex, int wordIndex);
};
