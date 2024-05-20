#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/State.h"
#include "chr/gl/shaders/ColorShader.h"

#include "common/NoiseSurfaceSpiral2.h"
#include "common/xf/FontManager.h"
#include "common/legacy/ui/ButtonLayout.h"

class Sketch : public chr::CrossSketch, public Button::Handler
{
public:
  Sketch();
  virtual ~Sketch() {}

  void setup() final;
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

  chr::gl::State state;

  float cameraOffset;
  glm::vec3 viewPosition;
  glm::quat viewOrientation;

  NoiseSurface surface;
  NoiseSurfaceSpiral2 spiral;
  chr::gl::VertexBatch<XYZ> spiralBatch;

  chr::xf::FontManager fontManager;
  std::shared_ptr<chr::xf::Font> font;
  chr::xf::FontSequence sequence;
  std::u16string text;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::ShaderProgram fogColorShader;
  chr::gl::ShaderProgram fogTextureAlphaShader;

  void generateSpiral();

  void buttonEvent(int id, Button::Action action) final;

  void enterFullScreen();
  void exitFullScreen();
  void fullScreenEvent(bool isFullScreen);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    static EM_BOOL fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
  #endif

  void initButtonLayout();
  void switchLanguage(const std::string &lang);
};
