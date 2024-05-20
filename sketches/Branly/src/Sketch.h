#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/State.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

#include "common/xf/FontManager.h"
#include "common/TextRectangle.h"
#include "common/legacy/ui/ButtonLayout.h"

class Sketch : public chr::CrossSketch, public Button::Handler
{
public:
  virtual ~Sketch() {}

  void setup() final;
  void resize() final;
  void update() final;
  void draw() final;

  void keyDown(int keyCode, int modifiers) final;
  void keyUp(int keyCode, int modifiers) final;

  void wheelUpdated(float offset);

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
  std::shared_ptr<chr::xf::Font> font;
  chr::gl::State textState;

  chr::TextRectangle rect;
  float ox[2] = { 0, 0 };
  float oy[2] = { 0, 0 };

  chr::gl::State decalState;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> decalBatch;
  chr::gl::Texture decalTexture;

  chr::gl::State fillState;
  chr::gl::IndexedVertexBatch<> fillBatch;

  chr::gl::State backgroundState;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> backgroundBatch;
  chr::gl::Texture backgroundTexture;

  State frameState;
  IndexedVertexBatch<> frameBatch;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureShader textureShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  float zoom, zoom2, defaultZoom;

  glm::vec2 pan;
  glm::vec2 dragOrigin;

  bool surfaceEnabled = true;

  glm::mat4 getTransform2D();
  glm::mat4 getTransform3D();
  void setZoom(float value);

  void drawMasks();
  void drawGeometry();

  void drawDecal(const chr::gl::Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy);
  void drawText(const chr::gl::Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy);

  void initTextures();
  void initBackground();

  void initFrame();
  void drawFrame();

  void buttonEvent(int id, Button::Action action) final;

  void enterFullScreen();
  void exitFullScreen();
  void fullScreenEvent(bool isFullScreen);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    static EM_BOOL fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
  #endif

  void initButtonLayout();
};
