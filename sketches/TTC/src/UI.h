#pragma once

#include "SharedGroup.h"
#include "PlaybackGroup.h"
#include "PlaybackScreen.h"
#include "LiveScreen.h"
#include "MainScreen.h"
#include "AboutScreen.h"

#include "chr/gl/BatchMap.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"

class UI : public std::enable_shared_from_this<UI>
{
public:
  struct Mouse
  {
    bool pressed;
    glm::vec2 position;
  };

  glm::vec2 windowSize;
  float scale;
  Mouse mouse;
  Button::Style defaultStyle;

  chr::gl::shaders::TextureAlphaShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  chr::xf::FontManager fontManager;

  std::shared_ptr<chr::xf::Font> mainFont;
  chr::xf::FontSequence mainFontSequence;

  std::shared_ptr<chr::xf::Font> subFont;
  chr::xf::FontSequence subFontSequence;

  chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> flatBatch;
  chr::gl::IndexedBatchMap<chr::gl::XYZ.UV.RGBA> batchMap;
  chr::gl::VertexBatch<chr::gl::XYZ.RGBA> lineBatch;

  std::map<std::string, chr::gl::Texture> textures;

  std::shared_ptr<Screen> currentScreen;
  std::shared_ptr<Screen> targetScreen;

  std::shared_ptr<PlaybackScreen> playbackScreen;
  std::shared_ptr<LiveScreen> liveScreen;
  std::shared_ptr<MainScreen> mainScreen;
  std::shared_ptr<AboutScreen> aboutScreen;

  std::shared_ptr<SharedGroup> sharedGroup;
  std::shared_ptr<PlaybackGroup> playbackGroup;

  float referenceH = 400;
  float mainFontSize = 20;
  float subFontSize = 10;
  float subFontShift = 1;
  float gap = 2;
  float navigatorY = 360;
  float navigatorH = 18;
  float navigatorPadding = 3;
  int navigatorSlotCount = 3;
  float deckY = navigatorY + navigatorH + gap;
  float deckH = referenceH - deckY - gap;

  void setup();
  void resize(const glm::vec2 &windowSize);

  void run();
  void draw();

  void mouseMoved(float x, float y);
  void mouseDragged(int button, float x, float y);
  void mousePressed(int button, float x, float y);
  void mouseReleased(int button, float x, float y);

  void setScreen(std::shared_ptr<Screen> screen);

  void beginGroupDraw();
  void endGroupDraw();

  void drawText(const std::u16string &text, const glm::vec2 &position, chr::XFont::Alignment alignX, float shiftY = 0);
  void drawTextInRect(const std::u16string &text, const chr::math::Rectf &bounds, float shiftY = 0);
  void drawFrame(const chr::math::Rectf &bounds, const glm::vec4 &color);

  glm::mat4 getScreenMatrix();

protected:
  void loadTexture(const std::string name, const fs::path &relativePath);
};
