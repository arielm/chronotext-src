#pragma once

#include "chr/cross/Context.h"

#include "UI.h"
#include "TreeManager.h"

#if defined(CHR_PLATFORM_EMSCRIPTEN)
  void fileLoaded(uintptr_t instance, const std::string &input);
#endif

class Sketch : public chr::CrossSketch
{
public:
  Sketch() = default;
  virtual ~Sketch() {}

  void setup() final;
  void resize() final;
  void update() final;
  void draw() final;

  void mouseMoved(float x, float y) final;
  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

  void keyPressed(uint32_t codepoint) final;
  void keyDown(int keyCode, int modifiers) final;

#if defined(CHR_PLATFORM_EMSCRIPTEN)
  void fileLoaded(const char *data, size_t size);
#endif

protected:
  std::shared_ptr<UI> ui;
  std::shared_ptr<TreeManager> treeManager;
};
