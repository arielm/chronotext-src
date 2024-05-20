#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;

Sketch::Sketch()
:
surface(400, 4, 0.04f, 3, 12345678),
spiralBatch(GL_LINE_STRIP, spiral.vertexBuffer)
{}

void Sketch::setup()
{
  surface.generateBase();

  spiral.enableWire(true);
  spiral.enablePath(true);
  spiral.setSampleSize(1.5f);
  spiral.setSamplingTolerance(0.0025f);

  spiral.path
    .setMode(FollowablePath3D::MODE_CLAMPED)
    .setSampling(FollowablePath3D::SAMPLING_CONTINUOUS);

  initButtonLayout(); // WILL INITIATE font

  // ---

  spiralState.glLineWidth(1);

  spiralBatch
    .setShader(colorShader)
    .setShaderColor(1, 1, 1, 0.25f);

  // ---

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    arcBall = ArcBall(
      windowInfo.size * 0.5f,
      fminf(windowInfo.width, windowInfo.height),
      53.3f,
      glm::quat(0.8428017f, -0.53769076f, -0.008445354f, -0.022421723f));

  buttonLayout.resize(windowInfo.size);
}

void Sketch::update()
{
    buttonLayout.update();
}

void Sketch::draw()
{
  glClearColor(0.333f, 0.333f, 0.333f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // ---

  auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.width / windowInfo.height, 1.0f, 1000.0f);
  auto mvpMatrix = projectionMatrix * arcBall.getMatrix();

  // ---

  generateSpiral(200 - clock()->getTime() * 5.0f);

  state
    .setShaderMatrix(mvpMatrix)
    .apply();

  spiralBatch.flush();
  font->replaySequence(sequence);

  // ---

  buttonLayout.draw();
}

void Sketch::generateSpiral(float offset)
{
  Matrix matrix;

  font->beginSequence(sequence);

  for (auto c : text)
  {
    int glyphIndex = font->getGlyphIndex(c);

    float half = font->getGlyphAdvance(glyphIndex) * 0.5f;
    offset += half;

    if (glyphIndex >= 0)
    {
      const auto value = spiral.path.offsetToValue(offset, half * 2);

      if (value.offset >= 0)
      {
        value.applyToMatrix(matrix);

        if (font->getDirection() < 0)
        {
          matrix.rotateZ(PI);
        }

        font->addGlyph(matrix, glyphIndex, -half * font->getDirection(), font->getOffsetY(XFont::ALIGN_MIDDLE));
      }
    }

    offset += half;
  }

  font->endSequence();
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
  else if (uiLock->check(this))
  {
    arcBall.mouseDragged(button, x, y);
  }
}

void Sketch::mousePressed(int button, float x, float y)
{
  buttonLayout.mousePressed(button, x, y);

  if (uiLock->acquire(this))
  {
    arcBall.mousePressed(button, x, y);
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
  else if (action == Button::ACTION_TOGGLED)
  {
    switch (id)
    {
      case 1:
        switchLanguage("EN");
        break;

      case 2:
        switchLanguage("FR");
        break;

      case 3:
        switchLanguage("HE");
        break;
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

  TiledAtlas atlas("atlas2.png", 64, 4);

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

  Button::Style style2;
  style2.backgroundColors[Button::STATE_NORMAL] = { 1, 0, 0, 1 };
  style2.colors[Button::STATE_NORMAL] = { 1, 1, 1, 1 };
  style2.backgroundColors[Button::STATE_TOGGLED] = { 1, 1, 1, 1 };
  style2.colors[Button::STATE_TOGGLED] = { 1, 0, 0, 1 };

  Button buttonL1;
  buttonL1
    .setId(1)
    .setStyle(style2)
    .setTextureBounds(atlas.getTileBounds(0, 1))
    .setType(Button::TYPE_TOGGLABLE);
  buttonLayout.addButton(ButtonLayout::LM, buttonL1);

  Button buttonL2;
  buttonL2
    .setId(2)
    .setStyle(style2)
    .setTextureBounds(atlas.getTileBounds(1, 1))
    .setType(Button::TYPE_TOGGLABLE);
  buttonLayout.addButton(ButtonLayout::LM, buttonL2);

  Button buttonL3;
  buttonL3
    .setId(3)
    .setStyle(style2)
    .setTextureBounds(atlas.getTileBounds(2, 1))
    .setType(Button::TYPE_TOGGLABLE);
  buttonLayout.addButton(ButtonLayout::LM, buttonL3);

  buttonLayout.toggleButton(1); // WILL INVOKE switchLanguage()
}

void Sketch::switchLanguage(const string &lang)
{
  string relativePath;

  if (lang == "EN")
  {
    relativePath = "Isaiah_40_EN.txt";
  }
  else if (lang == "FR")
  {
    relativePath = "Isaiah_40_FR.txt";
  }
  else if (lang == "HE")
  {
    relativePath = "Isaiah_40.txt";
  }

  text.clear();
  const auto &lines = utils::readLines<string>(InputSource::resource(relativePath));

  for (const auto &line : lines)
  {
    text.append(utils::to<u16string>(line));
    text.append(u" ");
  }

  if ((lang == "EN") || (lang == "FR"))
  {
    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d());

    font->setDirection(+1);
    font->setSize(2.5f);
  }
  else if (lang == "HE")
  {
    font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d());

    font->setDirection(-1);
    font->setSize(3);
  }

  font->setShader(textureAlphaShader);
  font->setColor(1, 1, 1, 1);

  spiral.setDirection(font->getDirection());
  spiral.setup(0, 120, 50, 2.5f, 2.5f);

  spiral.update(surface, 25);
}
