#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/gl/Batch.h"

#include "common/ArcBall.h"
#include "common/xf/FontManager.h"
#include "common/legacy/ui/ButtonLayout.h"

#include "Term.h"
#include "Block.h"
#include "SpiralPath.h"

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

  chr::gl::State state;

  chr::xf::FontManager fontManager;
  std::shared_ptr<chr::xf::Font> font;
  chr::xf::FontSequence sequence;

  chr::gl::shaders::ColorShader colorShader;
  chr::gl::shaders::TextureAlphaShader textureAlphaShader;

  ArcBall arcBall;

  chr::gl::Buffer<chr::gl::Vertex<>> vertexBuffer;
  chr::gl::IndexedVertexBatch<> capBatch;
  chr::gl::IndexedVertexBatch<> wallBatch;
  chr::gl::IndexedVertexBatch<> strokeBatch;

  std::vector<std::shared_ptr<Term>> termList;
  std::vector<std::shared_ptr<Term>> uniqueTermList;

  SpiralPath path;
  std::vector<Block> blocks;

  int minFreq, maxFreq;
  float heightFactor = 0;

  void parse(const fs::path &resourcePath);
  float getTermHeight(const Term &term) const;

  void generate();
  void generateText();
  void generateBlocks();
  void generateVertices();
  void generateCapIndices();
  void generateWallIndices();
  void generateStrokeIndices();

  void buttonEvent(int id, Button::Action action) final;

  void enterFullScreen();
  void exitFullScreen();
  void fullScreenEvent(bool isFullScreen);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    static EM_BOOL fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData);
  #endif

  void initButtonLayout();
};
