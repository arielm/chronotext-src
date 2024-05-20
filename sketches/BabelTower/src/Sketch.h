#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Camera.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"
#include "common/legacy/ui/ButtonLayout.h"

#include "TextHelix.h"

class Sketch : public chr::CrossSketch, public Button::Handler
{
public:
  virtual ~Sketch() {}

  void setup() final;
  void shutdown() final;

  void resize() final;
  void update() final;
  void draw() final;

  void mouseMoved(float x, float y) final;
  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

protected:
  UILock::Ref uiLock;
  ButtonLayout buttonLayout;

  bool fullScreen = false;
  bool fullScreenEnabled = false;

  chr::xf::FontManager fontManager;

  chr::gl::Camera camera;
  float azimuth;
  float h;
  float a;
  float v;
  bool collapsing;

  std::shared_ptr<chr::xf::Font> font;
  chr::xf::FontSequence sequence;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;
  std::u16string text;

  void reset();

  void buttonEvent(int id, Button::Action action) final;

  void enterFullScreen();
  void exitFullScreen();
  void fullScreenEvent(bool isFullScreen);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    static EM_BOOL fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
  #endif

  void initButtonLayout();
};
