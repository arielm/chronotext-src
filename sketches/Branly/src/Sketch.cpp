#include "Sketch.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

const float FONT_SIZE = 22;
const float LINE_H = 27;

const float marginRatio = 12 / 768.0f; // RELATIVE TO SCREEN-HEIGHT
const float zoomMin = 0.5f;
const float zoomMax = 4.0f;
const float zNear = 1, zFar = 10000;

const float originalW = 1024; // WIDTH OF THE PICTURE ORIGINALLY USED FOR ESTIMATION
const float eyeZ = 2185.0964f;
const glm::mat4 rotation = {0.5031331f, 0.31332415f, 0.80540985f, 0.0f, -0.009767972f, -0.9298407f, 0.36783275f, 0.0f, -0.8641537f, 0.19293606f, 0.46477312f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

const glm::vec3 center1 = { 187.0f, +208.0f, 0.0f };
const glm::vec3 center2 = { -233.0f, +208.0f, 0.0f };
const glm::vec3 center3 = { -233.0f, -249.0f, 0.0f };
const float my2 = 91.0f;
const float mx2 = -45;
const float my3 = 9.0f;
const float mx3 = 41.0f;
const float my4 = 63.0f;

const glm::vec3 pivot1 = { 187.0f, +208.0f, -169.0f };
const float angle1 = 3.8f;
const glm::vec3 pivot11 = { 111, 0, 0.0f };
const float angle11 = -2.39916f;
const glm::vec3 center6 = { 0.0f, 0.0f, 57.0f };
const glm::vec3 center7 = { 0.0f, -318.0f, 57.0f };
const glm::vec3 center9 = { 216.0f, 0.0f, 57.0f };
const float cy2 = -88.0f;

const glm::vec3 pivot2 = { -233.0f, +208.0f, -154.0f };
const float angle2 = 0.0f;
const glm::vec3 pivot22 = { -113, 0, 0 };
const float angle22 = -0.6f;
const glm::vec3 center11 = { 0, 0, 343 };
const glm::vec3 center12 = { -418, 0, 343 };
const glm::vec3 center14 = { 0, -594, 343 };
const float fy2 = -131.0f;
const float fx2 = -200.0f;
const float fy3 = -220.0f;
const float fx5 = 223;
const float fy5 = -129;
const float fx6 = 322;
const float fx7 = 409;

void Sketch::setup()
{
  initButtonLayout();
  initTextures();

  // ---

  DecalProperties decalProperties;
  decalProperties.shader = textureShader;
  decalProperties.color = { 1, 1, 1, 0.2f };
  decalProperties.texture = decalTexture;
  decalProperties.zoom = 1.2f; // XXX

  rect.setDecalProperties(decalProperties);
  decalProperties.assign(decalState, decalBatch);

  // ---

  FillProperties fillProperties;
  fillProperties.shader = colorShader;
  fillProperties.color = { 1, 1, 1, 0 };

  rect.setFillProperties(fillProperties);
  fillProperties.assign(fillState, fillBatch);

  // ---

  font = fontManager.getFont(InputSource::resource("Helvetica_Regular_64.fnt"), XFont::Properties3d());

  TextProperties textProperties;
  textProperties.color = { 1, 1, 1, 0.4f };
  textProperties.lineHeight = LINE_H;
  textProperties.font = font;
  textProperties.lineProvider = LineProvider::create(*font, FONT_SIZE, InputSource::resource("rock around the bunker.txt"));
  textProperties.fontSize = FONT_SIZE;

  rect.setTextProperties(textProperties);
  textState.setShader(textureAlphaShader);

  // ---

  initBackground();
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

  ox[0] = clock()->getTime() * 12; // XXX
  ox[1] = clock()->getTime() * 12; // XXX

  // ---

  backgroundState
    .setShaderMatrix(getTransform2D())
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
    glPolygonOffset(1, 1);

    fillBatch.clear();
    drawMasks();

    fillState
      .setShaderMatrix(getTransform3D())
      .apply();
    fillBatch.flush();

    glDepthMask(GL_FALSE);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // ---

    decalBatch.clear();

    xf::FontSequence sequence;
    font->beginSequence(sequence);

    drawGeometry();

    font->endSequence();

    textState
      .setShaderMatrix(getTransform3D())
      .apply();
    font->replaySequence(sequence);

    decalState
      .setShaderMatrix(getTransform3D())
      .apply();
    decalBatch.flush();
  }

  // ---

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
  float halfHeight = zNear * windowInfo.height / (2.0 * eyeZ * zoom);
  float halfWidth = halfHeight * windowInfo.width / windowInfo.height;
  glm::vec2 offset =(-pan / eyeZ * zNear) / zoom;

  auto projectionMatrix = glm::frustum(-halfWidth + offset.x, +halfWidth + offset.x, -halfHeight + offset.y, +halfHeight + offset.y, zNear, zFar);

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

void Sketch::drawMasks()
{
  Matrix matrix;

  // MIDDLE

  matrix
    .translate(center2.x, center3.y, center2.z);
  rect.drawFill(fillBatch, matrix, 0, 0, center1.x - center2.x, center1.y - center3.y);

  matrix
    .translate(0, center1.y - center3.y, 0)
    .rotateX(HALF_PI);
  rect.drawFill(fillBatch, matrix, 0, 0, center1.x - center2.x, pivot1.z);

  // CLOSE

  matrix
    .setIdentity()
    .translate(pivot1)
    .rotateY(angle1 * D2R);
  rect.drawFill(fillBatch, matrix, 0, 0, 200, 100);

  matrix
    .translate(pivot11)
    .rotateY(angle11 * D2R)
    .translate(center6.x, center7.y, center6.z);
  rect.drawFill(fillBatch, matrix, 0, 0, center9.x - center6.x, center6.y - center7.y);

  matrix
    .translate(0, center6.y - center7.y, 0)
    .rotateX(-HALF_PI);
  rect.drawFill(fillBatch, matrix, 0, 0, center9.x - center6.x, 60);

  // FAR

  matrix
    .setIdentity()
    .translate(pivot2.x - 200, pivot2.y, pivot2.z)
    .rotateY(angle2 * D2R);
  rect.drawFill(fillBatch, matrix, 0, 0, 200, 100);
}

void Sketch::drawGeometry()
{
  Matrix matrix;

  // MIDDLE

  float mx1 = center2.x;
  float my1 = center3.y;
  float mx5 = center1.x;
  float my5 = center1.y;
  float dy = -14;

  matrix
    .translate(0, 0, center2.z);
  drawDecal(matrix, mx1, my1, mx2, my2, 0, dy);
  drawDecal(matrix, mx2, my1, mx5, my3, 0, dy);
  drawDecal(matrix, mx3, my3, mx5, my4, 0, dy);
  drawText(matrix, mx1, my2, mx3, my5, 0, dy);
  drawText(matrix, mx2, my3, mx3, my2, 0, dy);
  drawText(matrix, mx3, my4, mx5, my5, 0, dy);

  matrix
    .translate(mx5, 0, 0)
    .rotateY(HALF_PI);
  drawDecal(matrix, 0, my1, 290, my4, mx5, dy);
  drawText(matrix, 0, my4, 265, my5, mx5, dy);

  // CLOSE

  float cx1 = center6.x;
  float cy1 = center7.y;
  float cx3 = center9.x;
  float cy3 = center6.y;
  dy = pivot1.y + 3;

  matrix
    .setIdentity()
    .translate(pivot1)
    .rotateY(angle1 * D2R)
    .translate(pivot11)
    .rotateY(angle11 * D2R)
    .translate(0, 0, center6.z);
  drawDecal(matrix, cx1, cy1, cx3, cy2, 0, dy);
  drawText(matrix, cx1, cy2, cx3, cy3, 0, dy);

  matrix
    .translate(cx3 - cx1, 0, 0)
    .rotateY(HALF_PI);
  drawDecal(matrix, 0, cy1, 95, cy2, cx3 - cx1, dy);
  drawText(matrix, 0, cy2, 70, cy3, cx3 - cx1, dy);

  // FAR

  float fx1 = center12.x;
  float fy1 = center14.y;
  float fx4 = center11.x;
  float fy4 = center11.y;
  dy = pivot2.y + 7; // XXX

  matrix
    .setIdentity()
    .translate(pivot2.x, pivot2.y, pivot2.z)
    .rotateY(angle2 * D2R)
    .translate(pivot22.x, pivot22.y, pivot22.z)
    .rotateY(angle22 * D2R)
    .translate(0, 0, center12.z);
  drawDecal(matrix, fx1, fy1, fx2, fy2, 0, dy);
  drawDecal(matrix, fx2, fy1, fx4, fy3, 0, dy);
  drawText(matrix, fx1, fy2, fx2, fy4, 0, dy);
  drawText(matrix, fx2, fy3, fx4, fy4, 0, dy);

  matrix
    .rotateY(HALF_PI);
  drawDecal(matrix, 0, fy1, fx5, fy3, fx4, dy);
  drawDecal(matrix, fx5, fy1, fx6, fy5, fx4, dy);
  drawText(matrix, 0, fy3, fx5, fy4, fx4, dy);
  drawText(matrix, fx5, fy5, fx7, fy4, fx4, dy);
}

void Sketch::drawDecal(const Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy)
{
  rect.drawDecal(decalBatch, matrix, x1, y1, x2 - x1, y2 - y1, ox[1] + dx, oy[1] + dy);
}

void Sketch::drawText(const Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy)
{
  rect.drawText(matrix, x1, y1, x2 - x1, y2 - y1, ox[0] + dx, oy[0] + dy);
}

void Sketch::initTextures()
{
  backgroundTexture = Texture(
    Texture::ImageRequest("286652175_1024.jpg")
      .setFlags(image::FLAGS_RBGA | image::FLAGS_POT));

  decalTexture = Texture(
    Texture::ImageRequest("camo.jpg")
      .setMipmap(true)
      .setAnisotropy(true)
      .setWrap(GL_REPEAT, GL_REPEAT));
}

void Sketch::initBackground()
{
  backgroundState
    .glDisable(GL_BLEND)
    .glDisable(GL_DEPTH_TEST)
    .glDisable(GL_CULL_FACE)
    .glDepthMask(GL_FALSE)
    .setShader(textureShader)
    .setShaderColor(1, 1, 1, 1);

  backgroundBatch
    .setTexture(backgroundTexture);

  draw::Sprite()
    .append(backgroundBatch);
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
