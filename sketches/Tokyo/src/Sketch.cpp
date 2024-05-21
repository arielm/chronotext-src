#include "Sketch.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

const float sy = 27.8f;
const float radius1 = 296.533f;
const float radius2 = 306.588f;
const float radius3 = 365.5f;
const glm::vec3 center1 = {-164.0f, +450.0f, 0.0f};
const glm::vec3 center2 = {74.0f, 0.0f, 0.0f};
const glm::vec3 center3 = {342.0f, -108.0f, -487.0f};
const glm::vec3 grid1 = {center1.x - radius1, center1.y, center1.z};
const glm::vec3 grid2 = {center2.x, center1.y, 0.0f};
const glm::vec3 guideY1 {center1.x, center1.y, -279.0f};
const float arc1 = (HALF_PI - asinf((radius1 + guideY1.z) / radius1)) + 0.001f; // XXX

const float FONT_SIZE[] = { sy * 3 / 4.0f, sy }; // EN + FR, HE
const float LINE_H = sy;

const float marginRatio = 12 / 768.0f; // RELATIVE TO SCREEN-HEIGHT
const float zoomMin = 0.5f;
const float zoomMax = 4.0f;
const float Z_NEAR = 250, Z_FAR = 12000;

const float originalW = 1024; // WIDTH OF THE PICTURE ORIGINALLY USED FOR ESTIMATION
const float eyeZ = 1048.8278f;
const glm::mat4 rotation = {0.94584435f, 0.1917415f, 0.26194203f, 0.0f, -0.095305234f, -0.6073367f, 0.7887072f, 0.0f, -0.3103149f, 0.7709587f, 0.556172f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

void Sketch::setup()
{
  initTextures();

  //

  DecalProperties decalProperties;
  decalProperties.shader = textureShader;
  decalProperties.color = { 1, 1, 1, 0.25f };
  decalProperties.texture = decalTexture;
  decalProperties.zoom = 2; // XXX

  rect.setDecalProperties(decalProperties);
  ring.setDecalProperties(decalProperties);
  decalProperties.assign(decalState, decalBatch);

  //

  initButtonLayout(); // WILL INITIATE font
  textState.setShader(textureAlphaShader);

  //

  initImage();
  initFrame();

  // ---

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  zoom2 = originalW / backgroundTexture.innerWidth;
  setZoom((windowInfo.size.y - marginRatio * windowInfo.size.y * 2) / backgroundTexture.innerHeight / zoom2); // XXX
  defaultZoom = zoom;

  buttonLayout.resize(windowInfo.size);
}

void Sketch::update()
{
  buttonLayout.update();
}

void Sketch::draw()
{
  glClearColor(0, 0, 0, 1);

  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  float u = fminf(1, clock()->getTime() * 0.333f);
  zFar = Z_NEAR + u * (Z_FAR - Z_NEAR);

  ox = font->getDirection() * clock()->getTime() * 20;
  oy = 1 * LINE_H + grid2.y - 13 * sy; // XXX

  // ---

  imageState
    .setShaderMatrix(getTransform2D())
    .glDisable(GL_BLEND)
    .apply();
  backgroundBatch.flush();

  // ---

  if (surfaceEnabled)
  {
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(8, 1);

    decalBatch.clear();

    drawingDecal = true;
    drawingText = false;
    drawGeometry();

    decalState
      .setShaderMatrix(getTransform3D())
      .apply();
    decalBatch.flush();

    glDepthMask(GL_FALSE);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // ---

    xf::FontSequence sequence;
    font->beginSequence(sequence);

    drawingDecal = false;
    drawingText = true;
    drawGeometry();

    font->endSequence();

    textState
      .setShaderMatrix(getTransform3D())
      .apply();
    font->replaySequence(sequence);
  }

  // ---

  imageState
    .glEnable(GL_BLEND)
    .apply();
  foregroundBatch.flush();

  drawFrame();

  // ---

  buttonLayout.draw();
}

glm::mat4 Sketch::getTransform2D()
{
  auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

  Matrix modelViewMatrix;
  modelViewMatrix
    .translate(windowInfo.size * 0.5f + pan)
    .scale(zoom * zoom2, -zoom * zoom2)
    .translate(-backgroundTexture.innerSize * 0.5f);

  return modelViewMatrix * projectionMatrix;
}

glm::mat4 Sketch::getTransform3D()
{
  float halfHeight = Z_NEAR * windowInfo.height / (2.0 * eyeZ * zoom);
  float halfWidth = halfHeight * windowInfo.width / windowInfo.height;
  glm::vec2 offset =(-pan / eyeZ * Z_NEAR) / zoom;

  auto projectionMatrix = glm::frustum(-halfWidth + offset.x, +halfWidth + offset.x, -halfHeight + offset.y, +halfHeight + offset.y, Z_NEAR, zFar);

  Matrix modelViewMatrix;
  modelViewMatrix.translate(0, 0, -eyeZ * zoom);
  modelViewMatrix *= rotation;
  modelViewMatrix.scale(zoom);

  return modelViewMatrix * projectionMatrix;
}

void Sketch::setZoom(float value)
{
  zoom = constrainf(roundf(value * zoom2 * 100) / (zoom2 * 100), zoomMin / zoom2, zoomMax / zoom2); // ROUNDING TO CLOSEST PERCENT
}

void Sketch::drawGeometry()
{
  float x, y, dx;
  Matrix matrix;

  // BOTTOM

  x = grid2.x - 24 * sy;
  y = grid2.y - 13 * sy;
  drawRect(matrix, x, y, x + 24 * sy, y + 13 * sy, 0, 0);

  x = grid2.x;
  matrix.setTranslate(0, 0, grid2.z - radius2);
  drawRing(matrix, x, y, 13 * sy, radius2, HALF_PI, sy / 2, 0, 0);

  // LEFT MIDDLE

  y = grid1.y - sy * (13 + 16) - 13 * sy;
  drawBalcon(y);

  // LEFT TOP

  y -= sy * (13 + 16);
  drawBalcon(y);

  // SIDE

  y = grid1.y - sy * (13 + 16) - (13 + 16 + 13 + 16) * sy;
  dx = grid1.x + arc1 * radius1;
  matrix.setTranslate(guideY1.x, 0, guideY1.z);
  matrix.rotateY(HALF_PI);
  drawRect(matrix, 0, y, 40 * sy, y + (4 + 13 + 16 + 13 + 16) * sy, dx, 0);

  // MASKED

  x = -35 * sy;
  y = grid1.y - sy * (13 + 16 - 4) - 4 * sy;
  matrix.setTranslate(guideY1.x, 0, guideY1.z);
  drawRect(matrix, x, y, x + 35 * sy, y + 4 * sy, dx, 0);
  //
  x = -9 * sy;
  y -= sy * (13 + 16);
  drawRect(matrix, x, y, x + 9 * sy, y + 16 * sy, dx, 0);
  //
  x = -5 * sy;
  y -= sy * (16 + 13);
  drawRect(matrix, x, y, x + 5 * sy, y + 16 * sy, dx, 0);

  // RIGHT

  y = center3.y - 13 * sy;
  matrix.setTranslate(center3.x, 0, center3.z - radius3);
  matrix.rotateY(-HALF_PI);
  drawRing(matrix, 0, y, 13 * sy, radius3, PI, sy / 2, 0, y - oy);
  //
  y -= sy * (13 + 11.5f);
  drawRing(matrix, 0, y, 13 * sy, radius3, PI, sy / 2, 0, y - oy - 13 * sy);
  //
  y -= sy * (13 + 16);
  drawRing(matrix, 0, y, 13 * sy, radius3, PI, sy / 2, 0, y - oy - 26 * sy);
}

void Sketch::drawBalcon(float y)
{
  Matrix matrix;
  matrix.translate(0, 0, grid1.z - radius1);
  drawRing(matrix, grid1.x, y, 13 * sy, radius1, arc1, sy / 2, 0, 0);

  matrix.translate(0, 0, radius1);
  drawRect(matrix, grid1.x - 38 * sy, y, grid1.x, y + 13 * sy, 0, 0);
}

void Sketch::drawRect(const Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy)
{
  if (drawingDecal)
  {
    rect.drawDecal(decalBatch, matrix, x1, y1, x2 - x1, y2 - y1, dx + ox, dy + oy);
  }
  else if (drawingText)
  {
    rect.drawText(matrix, x1, y1, x2 - x1, y2 - y1, dx + ox, dy + oy);
  }
}

void Sketch::drawRing(const Matrix &matrix, float x1, float y1, float h, float r, float a, float dd, float dx, float dy)
{
  if (drawingDecal)
  {
    ring.drawDecal(decalBatch, matrix, x1, y1, h, r, a, dd, dx + ox, dy + oy);
  }
  else if (drawingText)
  {
    ring.drawText(matrix, x1, y1, h, r, a, dx + ox, dy + oy);
  }
}

void Sketch::initTextures()
{
  backgroundTexture = Texture(
    Texture::ImageRequest("6980491_UN1_1024.jpg")
      .setFlags(image::FLAGS_POT));

  foregroundTexture = Texture(
    Texture::MaskedRequest("6980491_UN1_1024.jpg", "6980491_UN1_1024_MASK.png")
      .setFlags(image::FLAGS_RBGA | image::FLAGS_POT, image::FLAGS_TRANSLUCENT));

  decalTexture = Texture(
    Texture::ImageRequest("camo.jpg")
      .setMipmap(true)
      .setAnisotropy(true)
      .setWrap(GL_REPEAT, GL_REPEAT));
}

void Sketch::initImage()
{
  imageState
    .glDisable(GL_DEPTH_TEST)
    .glDisable(GL_CULL_FACE)
    .glDepthMask(GL_FALSE)
    .setShader(textureShader)
    .setShaderColor(1, 1, 1, 1);

  backgroundBatch
    .setTexture(backgroundTexture);

  draw::Sprite()
    .append(backgroundBatch);

  foregroundBatch
    .setTexture(foregroundTexture);

  draw::Sprite()
    .append(foregroundBatch);
}

void Sketch::initFrame()
{
  frameState
    .glDisable(GL_DEPTH_TEST)
    .glDisable(GL_CULL_FACE)
    .glDepthMask(GL_FALSE)
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 1);
}

void Sketch::drawFrame()
{
  frameState
    .setShaderMatrix(glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height))
    .apply();

  frameBatch.clear();

  auto size = backgroundTexture.innerSize * zoom * zoom2;
  auto p1 = (windowInfo.size - size) * 0.5f + pan;
  auto p2 = p1 + size;

  if (p1.x > 0)
  {
    draw::Rect()
      .setBounds(0, 0, p1.x, windowInfo.height)
      .append(frameBatch);
  }
  if (p2.x < windowInfo.width)
  {
    draw::Rect()
      .setBounds(p2.x, 0, windowInfo.width, windowInfo.height)
      .append(frameBatch);
  }
  if (p1.y > 0)
  {
    draw::Rect()
      .setBounds(0, 0, windowInfo.width, p1.y)
      .append(frameBatch);
  }
  if (p2.y < windowInfo.height)
  {
    draw::Rect()
      .setBounds(0, p2.y, windowInfo.width, windowInfo.height)
      .append(frameBatch);
  }

  frameBatch.flush();
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  #if defined(CHR_PLATFORM_DESKTOP) || defined(CHR_PLATFORM_EMSCRIPTEN)
    if (keyCode == KEY_SPACE)
    {
      surfaceEnabled = false;
    }
    else if (keyCode == KEY_RETURN)
    {
      pan = { 0, 0 };
      zoom = defaultZoom;
    }
  #endif
}

void Sketch::keyUp(int keyCode, int modifiers)
{
  #if defined(CHR_PLATFORM_DESKTOP) || defined(CHR_PLATFORM_EMSCRIPTEN)
    if (keyCode == KEY_SPACE)
    {
      surfaceEnabled = true;
    }
  #endif
}

void Sketch::wheelUpdated(float offset)
{
  setZoom(zoom + 0.01f * (offset > 0 ? -1 : +1)); // ONE PERCENT INCREMENT
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
    pan = glm::vec2(x, windowInfo.height - y) - dragOrigin;
  }
}

void Sketch::mousePressed(int button, float x, float y)
{
  buttonLayout.mousePressed(button, x, y);

  if (uiLock->acquire(this))
  {
    dragOrigin = glm::vec2(x, windowInfo.height - y) - pan;
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
  if ((lang == "EN") || (lang == "FR"))
  {
    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d());
    font->setDirection(+1);
  }
  else if (lang == "HE")
  {
    font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d());
    font->setDirection(-1);
  }

  TextProperties textProperties;
  textProperties.color = { 0, 0, 0, 1 };
  textProperties.lineHeight = LINE_H;
  textProperties.font = font;

  if (lang == "EN")
  {
    textProperties.lineProvider = LineProvider::create(*font, FONT_SIZE[0], InputSource::resource("Isaiah_29_EN.txt"));
    textProperties.fontSize = FONT_SIZE[0];
  }
  else if (lang == "FR")
  {
    textProperties.lineProvider = LineProvider::create(*font, FONT_SIZE[0], InputSource::resource("Isaiah_29_FR.txt"));
    textProperties.fontSize = FONT_SIZE[0];
  }
  else if (lang == "HE")
  {
    textProperties.lineProvider = LineProvider::create(*font, FONT_SIZE[1], InputSource::resource("Isaiah_29.txt"));
    textProperties.fontSize = FONT_SIZE[1];
  }

  rect.setTextProperties(textProperties);
  ring.setTextProperties(textProperties);
}
