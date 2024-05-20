#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;
using namespace math;

Sketch::Sketch()
:
surface(400, 4, 0.04f, 3, 12345678),
spiralBatch(GL_LINE_STRIP, spiral.vertexBuffer),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag"))
{}

void Sketch::setup()
{
  surface.generateBase();

  spiral.enableWire(true);
  spiral.enablePath(true);
  spiral.setSampleSize(1.5f);
  spiral.setSamplingTolerance(0.0025f);
  spiral.path.setSampling(FollowablePath3D::SAMPLING_CONTINUOUS);

  initButtonLayout(); // WILL INITIATE font

  // ---

  state
    .setShaderUniform("u_fogDensity", 0.0075f)
    .setShaderUniform("u_fogColor", 0.333f, 0.333f, 0.333f)
    .glLineWidth(1);

  spiralBatch
    .setShader(fogColorShader)
    .setShaderColor(1, 1, 1, 0.25f);

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
  glClearColor(0.333f, 0.333f, 0.333f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // ---

  const auto value = spiral.path.offsetToValue(cameraOffset);

  Matrix matrix;
  value.applyToMatrix(matrix);

  if (getElapsedFrames() == 1)
  {
    viewOrientation = matrix.getQuat();
    viewPosition = value.position;
  }
  else
  {
    glm::quat q1 = matrix.getQuat();
    glm::quat q2 = glm::slerp(viewOrientation, q1, 0.1f);
    viewOrientation = glm::normalize(viewOrientation + q2);

    viewPosition += (value.position - viewPosition) * 0.1f;
  }

  // ---

  auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.width / windowInfo.height, 1.0f, 1000.0f);

  Matrix modelViewMatrix;
  modelViewMatrix
    .scale(-1, 1, 1)
    .translate(0, 0, -70) /* DISTANCE */
    .rotateX(-30 * D2R) /* ELEVATION */
    .rotateZ((font->getDirection() > 0) ? (180 * D2R) : 0) /* AZIMUTH */
    .applyQuat<-1>(viewOrientation)
    .translate(-viewPosition);

  auto mvpMatrix = modelViewMatrix * projectionMatrix;

  // ---

  state
    .setShaderMatrix(modelViewMatrix * projectionMatrix)
    .apply();

  spiralBatch.flush();
  font->replaySequence(sequence);

  // ---

  State()
    .setShader(colorShader)
    .setShaderMatrix(mvpMatrix)
    .setShaderColor(1, 0, 0, 1)
    .glLineWidth(2)
    .apply();

  VertexBatch<>(GL_LINES)
    .addVertex(matrix.transformPoint(0, -1))
    .addVertex(matrix.transformPoint(0, +1))
    .flush();

  // ---

  float dt = 1 / 60.0f/*getFrameRate()*/;
  cameraOffset = boundf(cameraOffset + 7 * dt, font->getStringAdvance(text));

  // ---

  buttonLayout.draw();
}

void Sketch::generateSpiral()
{
  spiral.update(surface, 25);

  // ---

  float offset = 0;
  Matrix matrix;

  font->beginSequence(sequence);

  for (auto c : text)
  {
    int glyphIndex = font->getGlyphIndex(c);

    float half = font->getGlyphAdvance(glyphIndex) * 0.5f;
    offset += half;

    if (glyphIndex >= 0)
    {
      spiral.path
        .offsetToValue(offset, half * 2)
        .applyToMatrix(matrix);

      if (font->getDirection() < 0)
      {
        matrix.rotateZ(PI);
      }

      font->addGlyph(matrix, glyphIndex, -half * font->getDirection(), font->getOffsetY(XFont::ALIGN_MIDDLE));
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

  font->setShader(fogTextureAlphaShader);
  font->setColor(1, 1, 1, 1);

  spiral.setDirection(font->getDirection());
  spiral.setup(0, 120, 50, 0.25f, 3.0f);

  generateSpiral();

  cameraOffset = 0;
}
