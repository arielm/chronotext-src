#include "Sketch.h"

#include "chr/gl/Matrix.h"

using namespace std;
using namespace chr;
using namespace gl;

Sketch::Sketch()
:
surface(800, 4, 0.04f, 3, 12345678),
spiralBatch(GL_LINE_STRIP, spiral.vertexBuffer),
gridBatch(GL_LINES, grid.vertexBuffer, grid.indexBuffer)
{}

void Sketch::setup()
{
  initButtonLayout();

  // ---

  surface.generateBase();

  spiral.setup(0, 120, 20, 1.0f, 3.0f);
  grid.setup(surface, -120, -120, 240, 240);

  // ---

  spiralState.setShader(colorShader);
  spiralState.setShaderColor(1.0f, 0.25f, 0.0f, 0.3f);
  spiralState.glLineWidth(2);

  gridState.setShader(colorShader);
  gridState.setShaderColor(0, 0, 0, 0.4f);
  gridState.glLineWidth(1);

  // ---

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  buttonLayout.resize(windowInfo.size);
}

void Sketch::update()
{
  buttonLayout.update();
}

void Sketch::draw()
{
  glClearColor(1.0f, 0.9f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // ---

  auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.width / windowInfo.height, 1.0f, 1000.0f);

  Matrix modelViewMatrix;
  modelViewMatrix
    .scale(1, 1, 1)
    .translate(0, 0, -200) /* DISTANCE */
    .rotateX(-60 * D2R) /* ELEVATION */
    .rotateZ(45 * D2R); /* AZIMUTH */

  auto mvpMatrix = modelViewMatrix * projectionMatrix;

  spiralState.setShaderMatrix(mvpMatrix);
  gridState.setShaderMatrix(mvpMatrix);

  // ---

  float height = cosf(20 / 60.0f * clock()->getTime()) * 25;
  drawGrid(height);
  drawSpiral(height);

  // ---

  buttonLayout.draw();
}

void Sketch::drawSpiral(float height)
{
  spiral.update(surface, height, glm::zero<glm::vec2>());

  spiralState.apply();
  spiralBatch.flush();
}

void Sketch::drawGrid(float height)
{
  grid.update(surface, height, glm::zero<glm::vec2>());

  gridState.apply();
  gridBatch.flush();
}

void Sketch::mouseMoved(float x, float y)
{
  buttonLayout.mouseMoved(x, y);
}

void Sketch::mouseDragged(int button, float x, float y)
{
  if (uiLock->check(&buttonLayout))
  {
    buttonLayout.mouseDragged(button, x, y);
  }
}

void Sketch::mousePressed(int button, float x, float y)
{
  buttonLayout.mousePressed(button, x, y);
}

void Sketch::mouseReleased(int button, float x, float y)
{
  buttonLayout.mouseReleased(button, x, y);
  uiLock->release(this);
}

void Sketch::buttonEvent(int id, Button::Action action)
{
  if (id == 0)
  {
    if (fullScreen)
    {
      exitFullScreen();
    }
    else
    {
      enterFullScreen();
    }
  }
}

void Sketch::enterFullScreen()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    emscripten_request_fullscreen("#canvas", 1);
  #endif
}

void Sketch::exitFullScreen()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    emscripten_exit_fullscreen();
  #endif
}

void Sketch::fullScreenEvent(bool isFullScreen)
{
  fullScreen = isFullScreen;

  Button *button = buttonLayout.getButton(0);

  if (button)
  {
    if (isFullScreen)
    {
      button->setType(Button::TYPE_CLICKABLE);
      button->setTextureBounds(buttonLayout.atlas.getTileBounds(1, 0));
    }
    else
    {
      button->setType(Button::TYPE_PRESSABLE);
      button->setTextureBounds(buttonLayout.atlas.getTileBounds(0, 0));
    }
  }
}

#if defined(CHR_PLATFORM_EMSCRIPTEN)
  EM_BOOL Sketch::fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData)
  {
    reinterpret_cast<Sketch*>(userData)->fullScreenEvent(e->isFullscreen);
    return 0;
  }
#endif

void Sketch::initButtonLayout()
{
  uiLock = make_shared<UILock>();

  TiledAtlas atlas("atlas1.png", 64, 4);

  buttonLayout.setup(uiLock, atlas, 12);
  buttonLayout.setHandler(this);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    fullScreenEnabled = true;
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, 1, fullScreenChangeCallback);
  #endif

  //

  if (fullScreenEnabled)
  {
    Button::Style style1;
    style1.backgroundColors[Button::STATE_NORMAL] = { 1, 0, 0, 1 };
    style1.colors[Button::STATE_NORMAL] = { 1, 1, 1, 1 };
    style1.backgroundColors[Button::STATE_PRESSED] = { 0, 0, 0, 1 };
    style1.colors[Button::STATE_PRESSED] = { 1, 1, 1, 1 };

    Button buttonR1;
    buttonR1
      .setId(0)
      .setStyle(style1)
      .setTextureBounds(atlas.getTileBounds(0, 0))
      .setType(Button::TYPE_PRESSABLE);
    buttonLayout.addButton(ButtonLayout::RM, buttonR1);
  }
}
