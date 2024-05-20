#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;
using namespace math;

const float FORCE = 0.01f;

Sketch::Sketch()
:
surface(400, 4, 0.04f, 3, 12345678),
spiralBatch(GL_LINE_STRIP, spiral.vertexBuffer)
{}

void Sketch::setup()
{
  scaleFactor = windowInfo.height / 768.0f; // 768 WAS THE ORIGINAL SCREEN-HEIGHT

  // ---

  surface.generateBase();

  spiral.enableWire(true);
  spiral.enablePath(true);
  spiral.setSampleSize(1.5f);

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

  if (verseTarget != -1)
  {
    verseCurrent = verseTarget;
    verseTarget = -1;

    posValue = verseLengths[verseCurrent];
    posVel = 0;
  }
  else
  {
    verseCurrent = utils::search(verseLengths, posValue, 1, verseCount + 1);
  }

  posValue += posVel;
  posVel += posAcc;

  if (posValue > textLength)
  {
    posValue = textLength;
    posVel = 0;
  }

  if (posValue < 0.01f) // XXX
  {
    posValue = 0.01f; // XXX
    posVel = 0;
  }

  float heightFactor = 1 - (posValue / textLength);
  spiral.update(surface, 25 * heightFactor);

  font->setSize(mainTextSize);

  font->beginSequence(sequence[0]);
  generateSpiral(0, true);
  font->endSequence();

  font->beginSequence(sequence[1]);
  generateSpiral(0, false);
  font->endSequence();

  //

  font->setSize(titleTextSize);

  font->beginSequence(sequence[2]);
  drawTitle();
  font->endSequence();

  //

  const auto value = spiral.path.offsetToValue(posValue);

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

  State()
    .setShaderMatrix(getTransform3D())
    .glLineWidth(1)
    .apply();

  spiralBatch.flush();

  font->setColor(1, 1, 1, 1);
  font->replaySequence(sequence[0]);

  font->setColor(1, 1, 1, 0.667f);
  font->replaySequence(sequence[1]);

  // ---

  State()
    .setShader(colorShader)
    .setShaderMatrix(getTransform3D())
    .setShaderColor(1, 0.25f, 0, 1)
    .glLineWidth(4)
    .apply();

  VertexBatch<>(GL_LINES)
    .addVertex(matrix.transformPoint(0, -1))
    .addVertex(matrix.transformPoint(0, +1))
    .flush();

  // ---

  State()
    .setShaderMatrix(getTransform2D())
    .apply();

  font->setColor(0.8f, 0.8f, 0, 1);
  font->replaySequence(sequence[2]);

  // ---

  buttonLayout.draw();
}

glm::mat4 Sketch::getTransform2D()
{
  auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

  Matrix modelViewMatrix;
  modelViewMatrix
    .translate(0, windowInfo.height)
    .scale(+1, -1);

  return modelViewMatrix * projectionMatrix;
}

glm::mat4 Sketch::getTransform3D()
{
  auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.width / windowInfo.height, 1.0f, 1000.0f);

  Matrix modelViewMatrix;
  modelViewMatrix
    .scale(-1, 1, 1)
    .translate(0, 0, -45) /* DISTANCE */
    .rotateX(-37 * D2R) /* ELEVATION */
    .rotateZ((textDirection > 0) ? (180 * D2R) : 0) /* AZIMUTH */
    .applyQuat<-1>(viewOrientation)
    .translate(-viewPosition);

  return modelViewMatrix * projectionMatrix;
}

void Sketch::generateSpiral(float offset, bool drawCurrentVerse)
{
  Matrix matrix;

  for (auto index = 0; index < text.size(); index++)
  {
    bool on = (index >= verseIndex[verseCurrent] && index < verseIndex[verseCurrent + 1]);

    auto c = text[index];
    int glyphIndex = font->getGlyphIndex(c);

    float half = font->getGlyphAdvance(glyphIndex) * 0.5f;
    offset += half;

    if ((on && drawCurrentVerse) || (!on && !drawCurrentVerse))
    {
      if (glyphIndex >= 0)
      {
        const auto value = spiral.path.offsetToValue(offset, half * 2);

        if (value.offset >= 0)
        {
          value.applyToMatrix(matrix);

          if (textDirection < 0)
          {
            matrix.rotateZ(PI);
          }

          font->addGlyph(matrix, glyphIndex, -half * textDirection, font->getOffsetY(XFont::ALIGN_MIDDLE));
        }
      }
    }

    offset += half;
  }
}

void Sketch::drawTitle()
{
  int n = verseCurrent + 1;

  if (textDirection < 0)
  {
    auto title = getHebrewNumeral(n);

    float x = windowInfo.width - font->getStringAdvance(title) - 15 * scaleFactor;
    float y = 50 * scaleFactor;

    for (auto c : title)
    {
      auto glyphIndex = font->getGlyphIndex(c);
      x += font->getGlyphAdvance(glyphIndex);
      font->addGlyph(glyphIndex, x, y);
    }
  }
  else
  {
    auto title = utils::to<u16string>(utils::toString(n));

    float x = 15 * scaleFactor;
    float y = 50 * scaleFactor;

    for (auto c : title)
    {
      auto glyphIndex = font->getGlyphIndex(c);
      font->addGlyph(glyphIndex, x, y);
      x += font->getGlyphAdvance(glyphIndex);
    }
  }
}

u16string Sketch::getHebrewNumeral(int n)
{
  static constexpr char16_t decimals[9] = { 0x5d9, 0x5db, 0x5dc, 0x5d2, 0x5e0, 0x5e1, 0x5e2, 0x5e4, 0x5e6 };

  u16string result;

  if ((n > 0) && (n < 100))
  {
    int n10 = n / 10;
    int n1 = n - n10 * 10;

    if (n1 != 0)
    {
      result += 0x5d0 + char16_t(n1 - 1);
    }
    if (n10 != 0)
    {
      result += decimals[n10 - 1];
    }
  }

  return result;
}

void Sketch::prevVerse()
{
  verseTarget = (posValue != verseLengths[verseCurrent]) ? verseCurrent : constrain(verseCurrent - 1, 0, verseCount - 1);
}

void Sketch::nextVerse()
{
  verseTarget = constrain(verseCurrent + 1, 0, verseCount - 1);
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  switch (keyCode)
  {
    case KEY_LEFT:
      posAcc = -textDirection * FORCE;
      break;

    case KEY_RIGHT:
      posAcc = +textDirection * FORCE;
      break;

    case KEY_SPACE:
      posVel = 0;
      posAcc = 0;
      break;

    case KEY_UP:
      if (textDirection > 0) prevVerse(); else nextVerse();
      break;

    case KEY_DOWN :
      if (textDirection > 0) nextVerse(); else prevVerse();
      break;
  }
}

void Sketch::keyUp(int keyCode, int modifiers)
{
  posAcc = 0;
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
  if ((lang == "EN") || (lang == "FR"))
  {
    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d());

    textDirection = +1;
    mainTextSize = 2.5f;
    titleTextSize = 45 * scaleFactor;
  }
  else if (lang == "HE")
  {

    font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d());

    textDirection = -1;
    mainTextSize = 3;
    titleTextSize = 60 * scaleFactor;
  }

  font->setDirection(textDirection);
  font->setSize(mainTextSize);
  font->setShader(textureAlphaShader);

  //

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

  const auto &lines = utils::readLines<string>(InputSource::resource(relativePath));

  verseCount = lines.size();
  int charCounter = 0;

  text.clear();
  verseIndex.clear();
  verseLengths.clear();

  for (auto i = 0; i < verseCount; i++)
  {
    text.append(utils::to<u16string>(lines[i]));

    if (i < verseCount - 1)
    {
      text.append(u" ");
    }

    verseIndex.push_back(charCounter);
    verseLengths.push_back(font->getSubStringAdvance(text, 0, charCounter));

    charCounter = text.size();
  }

  verseIndex.push_back(charCounter);
  verseLengths.push_back(font->getSubStringAdvance(text, 0, charCounter));

  textLength = font->getStringAdvance(text);

  //

  spiral.setDirection(font->getDirection());
  spiral.setup(2, 122, 50, 0.25f, 6);

  posValue = 0;
  posVel = 0;
}
