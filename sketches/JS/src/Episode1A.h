#pragma once

#include "Episode.h"

#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/SeaSurfaceSpiral1.h"
#include "common/legacy/View.h"
#include "common/legacy/FogManager.h"
#include "common/legacy/FadeManager.h"
#include "common/legacy/Helix.h"

class Episode1A : public Episode
{
public:
  Episode1A(const Episode::Context &context, Episode::Lang lang);

  void setup() override;
  void enter() override;

  void update() override;
  void draw() override;

protected:
  std::shared_ptr<chr::XFont> font;
  std::u16string line;

  SeaSurface surface;
  SeaSurfaceSpiral1 spiral1, spiral2;

  Helix helix;
  chr::xf::FontSequence sequence;

  float azimuth1;
  float azimuth2;

  float fontSize1;
  float fontSize2;
  int textDir;
  bool mirror;

  int step;

  View view;
  FadeManager fadeManager;
  FogManager fogManager;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;
};
