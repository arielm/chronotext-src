#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/Batch.h"
#include "chr/time/Clock.h"

#include "common/Noise.h"

class Sketch : public chr::CrossSketch
{
public:
  Sketch();

  void setup() final;
  void draw() final;

  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;

protected:
  Noise noise;

  chr::gl::VertexBatch<chr::gl::XYZ> lineBatch;
  chr::gl::shaders::ColorShader colorShader;

  chr::Clock::Ref animClock = chr::Clock::create();

  void reseed(int64_t seed);
  void updateSpiral();
};
