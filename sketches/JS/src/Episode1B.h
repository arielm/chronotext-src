#pragma once

#include "Episode.h"

#include "common/legacy/View.h"
#include "common/legacy/FogManager.h"
#include "common/legacy/FadeManager.h"
#include "common/legacy/TextNoisePatch.h"
#include "common/legacy/Helix.h"

class Episode1B : public Episode
{
public:
  Episode1B(const Episode::Context &context, Episode::Lang lang);

  void setup() override;
  void enter() override;
  void leave() override;

  void update() override;
  void draw() override;

protected:
  std::shared_ptr<chr::XFont> font;
  std::shared_ptr<LineProvider> lineProvider;
  std::u16string line;

  NoiseSurface surface;
  TextNoisePatch patch;
  Helix helix;

  chr::xf::FontSequence sequenceSoil;
  chr::xf::FontSequence sequenceCamps;
  chr::xf::FontSequence sequenceColumn;

  float viewOX;
  float viewOY;

  float textOX;
  float textOY;

  float fontSize;
  float fontSize1;
  float fontSize2;
  int textDir;
  bool mirror;

  float azimuth;

  int turn;
  int step;

  View view;
  FadeManager fadeManager;
  FogManager fogManager;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;

  void drawCamps(float x, float y, float d2, float d3);
};
