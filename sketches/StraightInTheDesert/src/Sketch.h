#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/gl/Batch.h"

#include "common/NoiseSurfaceSpiral1.h"
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

  void keyDown(int keyCode, int modifiers) final;
  void keyUp(int keyCode, int modifiers) final;

  void mouseMoved(float x, float y) final;
  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

protected:
  UILock::Ref uiLock;
  ButtonLayout buttonLayout;

  bool fullScreen = false;
  bool fullScreenEnabled = false;

  NoiseSurface surface;
  NoiseSurfaceSpiral1 spiral;
  chr::gl::State spiralState;
  chr::gl::VertexBatch<XYZ> spiralBatch;

  chr::xf::FontManager fontManager;
  std::shared_ptr<chr::xf::Font> font;
  chr::xf::FontSequence sequence[3];

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  glm::vec3 viewPosition;
  glm::quat viewOrientation;
  float scaleFactor;

  float posValue = 0.01f; // XXX
  float posVel = 0;
  float posAcc = 0;

  std::vector<int> verseIndex;
  int verseCount;
  int verseCurrent = 0;
  int verseTarget = -1;
  std::vector<float> verseLengths;

  std::u16string text;
  float textLength;
  float textDirection;
  float mainTextSize;
  float titleTextSize;

  glm::mat4 getTransform2D();
  glm::mat4 getTransform3D();

  void generateSpiral(float offset, bool drawCurrentVerse);

  void drawTitle();
  std::u16string getHebrewNumeral(int n);

  void prevVerse();
  void nextVerse();

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
