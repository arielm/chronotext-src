#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/gl/Batch.h"
#include "chr/time/Clock.h"

#include "common/NoiseSurfaceSpiral2.h"
#include "common/xf/FontManager.h"

class Sketch : public chr::CrossSketch
{
public:
  Sketch();

  void setup() final;
  void draw() final;

  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;

protected:
  NoiseSurface surface;
  NoiseSurfaceSpiral2 spiral;
  chr::gl::VertexBatch<XYZ> lineBatch;

  chr::xf::FontManager fontManager;
  std::shared_ptr<chr::XFont> font;
  chr::xf::FontSequence sequence;
  std::u16string text;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  chr::Clock::Ref animClock = chr::Clock::create();

  void generateSpiral();
  void reseed(int64_t seed);
};
