#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace xf;
using namespace gl;

static constexpr float FOVY = 45;
static constexpr float CAMERA_DISTANCE = 180;
static constexpr float CAMERA_ELEVATION = -85;
static constexpr float CAMERA_AZIMUTH = -215;

static constexpr float R1 = 25;
static constexpr float R2 = 50;
static constexpr float H = 100;
static constexpr float A = -0.4f;

static constexpr float FONT_SIZE = 4.5f;

static constexpr float PAN_X = 0;
static constexpr float PAN_Y = 20;
static constexpr float ZOOM = 1.0f;
static constexpr float REFERENCE_H = 480;

void Sketch::setup()
{
  initButtonLayout();

  // ---

  text = utils::readText<u16string>(InputSource::resource("Genesis 11-1.txt"));

  font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d());
  font->setSize(FONT_SIZE);

  // ---

  azimuth = CAMERA_AZIMUTH;

  reset();

  // ---

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::shutdown()
{
  fontManager.shutdown();
}

void Sketch::resize()
{
  camera
    .setFov(FOVY)
    .setClip(1, 1000)
    .setWindowSize(windowInfo.size)
    .setPan2D(glm::vec2(PAN_X, PAN_Y) * (windowInfo.size.y / REFERENCE_H))
    .setZoom2D(ZOOM);

  buttonLayout.resize(windowInfo.size);
}

void Sketch::update()
{
  buttonLayout.update();

  if (collapsing)
  {
    v += a / 60/*getFrameRate()*/;
    h = max<float>(15, h + v);

    if (h <= 15)
    {
      v /= -1.667f;

      if (v < 0.005f)
      {
        reset();
      }
    }
  }
}

void Sketch::draw()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // ---

  camera.getViewMatrix()
    .setIdentity()
    .scale(1, -1, 1)
    .translate(0, 0, -CAMERA_DISTANCE)
    .rotateX(-CAMERA_ELEVATION * D2R)
    .rotateZ(-azimuth * D2R)
    .translate(0, 0, -(H / 2 - h));

  // ---

  if (collapsing)
  {
    font->beginSequence(sequence);
    TextHelix::draw(*font, text, R1, R2, -h);
    font->endSequence();
  }

  State()
    .setShader(textureAlphaShader)
    .setShaderMatrix(camera.getViewProjectionMatrix())
    .apply();

  glEnable(GL_CULL_FACE);

  glCullFace(GL_FRONT);
  font->setColor(1, 1, 1, 0.333f);
  font->replaySequence(sequence);

  glCullFace(GL_BACK);
  font->setColor(1, 1, 1, 1.0f);
  font->replaySequence(sequence);

  glDisable(GL_CULL_FACE);

  azimuth -= 0.5; // XXX

  // ---

  buttonLayout.draw();
}

void Sketch::reset()
{
  font->beginSequence(sequence);
  TextHelix::draw(*font, text, R1, R2, -H);
  font->endSequence();

  h = H;
  a = A;
  v = 0;
  collapsing = false;
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

  if (uiLock->acquire(this))
  {
    collapsing = true;
  }
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
