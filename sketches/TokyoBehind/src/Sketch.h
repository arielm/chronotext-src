#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureShader.h"

#include "common/TextRectangle.h"
#include "common/TextRing.h"
#include "common/legacy/ui/ButtonLayout.h"

class Sketch : public chr::CrossSketch, public Button::Handler
{
public:
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

  chr::TextRectangle rect;
  chr::TextRing ring;
  bool drawingGrid;
  bool drawingDecal;
  bool drawingFill;
  float ox = 0, oy = 0;
  bool showImage = false;
  float zoom, zoom2;
  float travellingOffset;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureShader textureShader;

  chr::gl::State decalState;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> decalBatch;
  chr::gl::Texture decalTexture;

  chr::gl::State fillState;
  chr::gl::IndexedVertexBatch<> fillBatch;
  chr::gl::IndexedVertexBatch<> maskBatch;

  chr::gl::State gridState;
  chr::gl::VertexBatch<> gridBatch;

  chr::gl::State imageState;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> foregroundBatch, backgroundBatch;
  chr::gl::Texture foregroundTexture, backgroundTexture;

  State frameState;
  IndexedVertexBatch<> frameBatch;

  glm::mat4 getTransform2D();
  glm::mat4 getTransform3D();
  void setZoom(float value);

  void drawContent();
  void drawGeometry();
  void drawBalcon(float y);

  void drawRect(const chr::gl::Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy);
  void drawRing(const chr::gl::Matrix &matrix, float x1, float y1, float h, float r, float a, float dd, float dx, float dy);

  void initTextures();
  void initImage();

  void initFrame();
  void drawFrame();
  chr::math::Rectf getFrame() const;

  void buttonEvent(int id, Button::Action action) final;

  void enterFullScreen();
  void exitFullScreen();
  void fullScreenEvent(bool isFullScreen);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    static EM_BOOL fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
  #endif

  void initButtonLayout();
};
