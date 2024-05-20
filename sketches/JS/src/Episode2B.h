#pragma once

#include "Episode.h"

#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/legacy/View.h"
#include "common/legacy/FogManager.h"
#include "common/legacy/FadeManager.h"
#include "common/legacy/TextNoisePatch.h"

class Episode2B : public Episode
{
public:
  Episode2B(const Episode::Context &context, Episode::Lang lang);

  void setup() override;
  void enter() override;

  void update() override;
  void draw() override;

protected:
  std::shared_ptr<chr::XFont> font;
  chr::xf::FontSequence sequence;
  std::shared_ptr<LineProvider> lineProvider;

  NoiseSurface surface;
  TextNoisePatch patch;

  float viewOX;
  float viewOY;

  float textOX;
  float textOY;

  float fontSize;
  int textDir;
  bool mirror;

  float terrainH;

  int step;

  View view;
  FadeManager fadeManager;
  FogManager fogManager;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;
};
