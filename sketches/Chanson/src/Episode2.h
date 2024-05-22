#pragma once

#include "Episode.h"
#include "BrickLine.h"

#include "chr/gl/Batch.h"

class Episode2 : public Episode
{
public:
  Episode2(Sketch *sketch);

  void setup() final;
  void update() final;
  void draw() final;

  void start(double rate) final;

  bool isPlaying() final;
  void togglePlay() final;
  void setPlayingRate(double rate) final;

protected:
  unsigned int startOffset;
  unsigned int endOffset;

  std::vector<BrickLine> brickLines;

  BrickLine *referenceBrickLine;
  std::vector<Brick2>::const_iterator referenceBrick;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;

  chr::gl::IndexedVertexBatch<> capBatch;
  chr::gl::IndexedVertexBatch<> wallBatch;
  chr::gl::IndexedVertexBatch<> strokeBatch;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> vertexBuffer;
  chr::gl::Buffer<GLuint> capIndexBuffer;
  chr::gl::Buffer<GLuint> wallIndexBuffer;
  chr::gl::Buffer<GLuint> strokeIndexBuffer;

  chr::xf::FontSequence fontSequence;

  float brickHeight;
  float brickDepth;
  float brickPadding;
  float brickMargin;

  float nearClip;
  float farClip;

  float visualToSoundFactor;
  float scrollingOffset;

  void setupLines();
  void setupBricks(float startOffset, float endOffset);
  void updateBricks();
  std::vector<Brick2>::iterator findBrick(unsigned int offset);
  std::vector<Brick2>::const_iterator findBrick(int stanzaIndex, int verseIndex, int wordIndex);
};
