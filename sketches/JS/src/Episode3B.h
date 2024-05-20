#pragma once

#include "Episode.h"

#include "common/SeaSurfaceSpiral1.h"
#include "common/legacy/View.h"
#include "common/legacy/FogManager.h"
#include "common/legacy/FadeManager.h"
#include "common/legacy/TextNoisePatch.h"
#include "common/legacy/SeaPatch.h"

class Episode3B : public Episode
{
public:
  Episode3B(const Episode::Context &context, Episode::Lang lang);

  void setup() override;
  void enter() override;

  void update() override;
  void draw() override;

protected:
  std::shared_ptr<chr::XFont> font;
  chr::xf::FontSequence sequence;
  std::shared_ptr<LineProvider> lineProvider;

  NoiseSurface noiseSurface;
  TextNoisePatch noisePatch;

  SeaSurface seaSurface;
  SeaPatch seaPatch;
  SeaSurfaceSpiral1 seaSpiral;

  float viewOX;
  float viewOY;

  float textOX;
  float textOY;

  float fontSize;
  int textDir;
  bool mirror;

  float level;
  int step;

  View view;
  FadeManager fadeManager;
  FogManager fogManager;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;

  float cliffNoiseBase(float x, float y);
  static float posterize(float x, float factor);
};
