#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"

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

  void mouseMoved(float x, float y) final;
  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

protected:
  UILock::Ref uiLock;
  ButtonLayout buttonLayout;

  bool fullScreen = false;
  bool fullScreenEnabled = false;

  chr::TextRectangle rect[4];
  float ox = 0, oy = 0;

  bool showBackground = false;
  float zoom, zoom2;
  float cyf, cym, cyc;
  float mx6, cx4, fx7;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureShader textureShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  chr::gl::State decalState[2];
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> decalBatch[2];
  chr::gl::Texture decalTexture[2];

  chr::gl::IndexedVertexBatch<> fillBatch;
  chr::gl::VertexBatch<> lineBatch;

  chr::gl::State gridState;
  chr::gl::VertexBatch<> gridBatch;

  chr::gl::State backgroundState;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> backgroundBatch;
  chr::gl::Texture backgroundTexture;

  State frameState;
  IndexedVertexBatch<> frameBatch;

  glm::mat4 getTransform2D();
  glm::mat4 getTransform3D();
  void setZoom(float value);

  void drawContent();

  void middleWall(chr::gl::Matrix &matrix, bool up);
  void closeWall(chr::gl::Matrix &matrix, bool up);
  void farWall(chr::gl::Matrix &matrix, bool up);

  void middleMap(chr::gl::Matrix &matrix, bool up);
  void closeMap(chr::gl::Matrix &matrix, bool up);
  void farMap(chr::gl::Matrix &matrix, bool up);

  void drawDecal(const chr::gl::Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy);
  void drawGrid(const chr::gl::Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy);

  void maskQuad(const chr::gl::Matrix &matrix, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
  void quad(const chr::gl::Matrix &matrix, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &p4);
  void quadCCW(const chr::gl::Matrix &matrix, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &p4);
  void segment(const chr::gl::Matrix &matrix, const glm::vec3 &p1, const glm::vec3 &p2);

  void initTextures();
  void initBackground();

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
