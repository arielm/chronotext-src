#pragma once

#include "Episode.h"

#include "common/legacy/View.h"
#include "common/legacy/FogManager.h"
#include "common/legacy/FadeManager.h"
#include "common/legacy/TextNoisePatch.h"
#include "common/legacy/Helix.h"

class Episode3A : public Episode
{
public:
  Episode3A(const Episode::Context &context, Episode::Lang lang);
  ~Episode3A();

  void setup() override;
  void enter() override;

  void update() override;
  void draw() override;

protected:
  std::shared_ptr<chr::XFont> font;
  std::shared_ptr<LineProvider> lineProvider;
  std::u16string line;

  NoiseSurface surface;
  TextNoisePatch *patches;
  chr::xf::FontSequence *sequencesSoil;

  Helix helix;
  chr::xf::FontSequence sequenceColumn;
  chr::xf::FontSequence sequenceTracks;

  float textOX;
  float textOY;

  int currentColumnGroupIndex;
  int currentSoilIndex;

  std::vector<std::vector<float>> columnPathIndex;
  std::vector<std::vector<float>> columnY;

  std::vector<float> columnX0;
  std::vector<float> columnX;
  std::vector<float> columnVX;
  std::vector<float> columnAZ;
  std::vector<float> columnVA;

  float fontSize;
  float fontSize1;
  float fontSize2;
  int textDir;
  bool mirror;

  int step;

  View view;
  FadeManager fadeManager;
  FogManager fogManager;

  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;

  void drawSoil(chr::gl::Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix, int soilIndex);
  void drawTracks(int columnGroupIndex, int soilIndex);
  void drawTrack(int columnGroupIndex, int soilIndex, int row, float x);
  void drawColumns(chr::gl::Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix, int columnGroupIndex, int soilIndex);
  void drawColumn(chr::gl::Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix, int columnGroupIndex, int soilIndex, float x, float y, float az);

  void resetColumns(int columnGroupIndex);
  void allocateColumnGroup(int columnGroupIndex, int soilIndex);
};
