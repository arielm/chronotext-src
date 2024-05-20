#pragma once

#include "Episode.h"

#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/legacy/View.h"
#include "common/legacy/FogManager.h"
#include "common/legacy/FadeManager.h"
#include "common/legacy/TextArc.h"
#include "common/legacy/TextRectangle.h"
#include "common/legacy/TextNoisePatch.h"

class Episode2A : public Episode
{
public:
  Episode2A(const Episode::Context &context, Episode::Lang lang);

  void setup() override;
  void enter() override;
  void leave() override;

  void update() override;
  void draw() override;

protected:
  std::shared_ptr<chr::XFont> font;
  std::shared_ptr<LineProvider> lineProviderSoil;
  std::shared_ptr<LineProvider> lineProviderGate;

  NoiseSurface surface;
  TextNoisePatch patch;

  TextRectangle rectangle1;
  TextRectangle rectangle2;
  TextArc arc1;
  TextArc arc2;

  chr::xf::FontSequence sequenceSoil1, sequenceSoil2;
  chr::xf::FontSequence sequenceGate;

  float textSoilOX;
  float textSoilOY;

  float textGateOX;
  float textGateOY;

  float fontSoilSize;
  float fontGateSize;
  int textDir;
  bool mirror;

  int turn;
  int step;

  View view;
  FadeManager fadeManager;
  FogManager fogManager;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;
  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  void drawSoil(chr::gl::Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix);
  void drawGate(chr::gl::Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix);
};
