#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/Batch.h"

#include "common/NoiseSurfaceSpiral1.h"
#include "common/NoiseSurfaceGrid.h"
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

  NoiseSurface surface;

  NoiseSurfaceSpiral1 spiral;
  chr::gl::State spiralState;
  chr::gl::VertexBatch<> spiralBatch;

  NoiseSurfaceGrid grid;
  chr::gl::State gridState;
  chr::gl::IndexedVertexBatch<> gridBatch;

  chr::gl::shaders::ColorShader colorShader;

  void drawSpiral(float height);
  void drawGrid(float height);

  void buttonEvent(int id, Button::Action action) final;

  void enterFullScreen();
  void exitFullScreen();
  void fullScreenEvent(bool isFullScreen);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    static EM_BOOL fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
  #endif

  void initButtonLayout();
};
