#pragma once

#include "common/legacy/ui/Button.h"
#include "common/legacy/ui/TiledAtlas.h"
#include "common/legacy/ui/UILock.h"

#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/time/Clock.h"

#include <map>

class ButtonLayout
{
public:
  enum Layout
  {
    LM,
    RM
  };

  TiledAtlas atlas;

  void setup(UILock::Ref uiLock, const TiledAtlas &atlas, int padding);
  void resize(const glm::vec2 &size);
  void update();
  void draw();

  void setHandler(Button::Handler *handler);
  void addButton(Layout layout, const Button &button);
  Button* getButton(int id);
  void toggleButton(int id);

  void mouseMoved(float x, float y);
  void mouseDragged(int button, float x, float y);
  void mousePressed(int button, float x, float y);
  void mouseReleased(int button, float x, float y);

protected:
  int padding = 0;

  UILock::Ref uiLock;
  Button::Handler *handler = nullptr;
  chr::Clock::Ref clock;
  glm::vec2 windowSize;
  bool buttonsHidden = false;

  std::map<Layout, std::vector<Button>> cartridges;
  std::map<int, Layout> layoutMap;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> fillBatch;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV.RGBA> textureBatch;

  float getDimension(std::vector<Button> &cartridge);
  void layoutVertically(std::vector<Button> &cartridge, float x, float y);

  void handleButtonClicked(Button *button);
  void handleButtonPressed(Button *button);
  void handleButtonToggled(Button *button);

  Button* getClosestButton(const glm::vec2 &position);
  Button* getArmedButtonByIndex(int index);
};
