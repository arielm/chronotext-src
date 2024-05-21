#include "Sketch.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Circle.h"

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
const float arc1 = (HALF_PI - asinf((radius1 + guideY1.z) / radius1)) + 0.01f; // XXX

const float marginRatio = 12 / 768.0f; // RELATIVE TO SCREEN-HEIGHT
const float zoomMin = 0.5f;
const float zoomMax = 4.0f;
const float zNear = 250, zFar = 7500;

const float originalW = 1024; // WIDTH OF THE PICTURE ORIGINALLY USED FOR ESTIMATION
const float eyeZ = 1048.8278f;
const glm::mat4 rotation = {0.94584435f, 0.1917415f, 0.26194203f, 0.0f, -0.095305234f, -0.6073367f, 0.7887072f, 0.0f, -0.3103149f, 0.7709587f, 0.556172f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

void Sketch::setup()
{
  initButtonLayout();
  initTextures();

  // ---

  DecalProperties decalProperties;
  decalProperties.shader = textureShader;
  decalProperties.color = { 1, 1, 1, 0.5f };
  decalProperties.texture = decalTexture;
  decalProperties.zoom = sy / 16;

  rect.setDecalProperties(decalProperties);
  ring.setDecalProperties(decalProperties);
  decalProperties.assign(decalState, decalBatch);

  // ---

  FillProperties fillProperties;
  fillProperties.shader = colorShader;
  fillProperties.color = { 1, 1, 1, 1 };

  rect.setFillProperties(fillProperties);
  ring.setFillProperties(fillProperties);
  fillProperties.assign(fillState, fillBatch);

  // ---

  GridProperties gridProperties;
  gridProperties.shader = colorShader;
  gridProperties.color = { 0, 0, 0, 0.4f };
  gridProperties.lineWidth = 1;
  gridProperties.gridSize = sy;
  gridProperties.horizontal = true;
  gridProperties.vertical = false;

  rect.setGridProperties(gridProperties);
  ring.setGridProperties(gridProperties);
  gridProperties.assign(gridState, gridBatch);

  gridBatch.setPrimitive(GL_LINES);

  // ---

  maskBatch.setShaderColor(0, 0, 0, 1);

  initImage();
  initFrame();

  // ---

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  zoom2 = originalW / foregroundTexture.innerWidth;
  setZoom((windowInfo.size.y - marginRatio * windowInfo.size.y * 2) / foregroundTexture.innerHeight / zoom2); // XXX

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

  float t = clock()->getTime();
  float u1 = fminf(1, t * 0.1f);
  travellingOffset = (1 - u1) * 456;

  if (t < 10)
  {
      showImage = false;
    drawContent();
  }
  else if (t < 15)
  {
    auto frame = getFrame();

    float u2 = fminf(1, (t - 10) / 5);
    float clipX = (1 - u2) * windowInfo.width;
    oy = -6;

    glEnable(GL_SCISSOR_TEST);

    glScissor(0, 0, (int)clipX, windowInfo.height);
      showImage = false;
    drawContent();

    glScissor(int(clipX), 0, windowInfo.width, windowInfo.height);
      showImage = true;
    drawContent();

    glDisable(GL_SCISSOR_TEST);
  }
  else
  {
    oy = -6 - 8 * (t - 15); // XXX

    showImage = true;
    drawContent();
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
    .translate(windowInfo.size * 0.5f)
    .scale(zoom * zoom2, -zoom * zoom2)
    .translate(-foregroundTexture.innerSize * 0.5f);

  return modelViewMatrix * projectionMatrix;
}

glm::mat4 Sketch::getTransform3D()
{
  float halfHeight = zNear * windowInfo.height / (2.0 * eyeZ * zoom);
  float halfWidth = halfHeight * windowInfo.width / windowInfo.height;
  auto projectionMatrix = glm::frustum(-halfWidth, +halfWidth, -halfHeight, +halfHeight, zNear, zFar);

  Matrix modelViewMatrix;
  modelViewMatrix.translate(travellingOffset * zoom, 0, -eyeZ * zoom);
  modelViewMatrix *= rotation;
  modelViewMatrix.scale(zoom);

  return modelViewMatrix * projectionMatrix;
}

void Sketch::setZoom(float value)
{
  zoom = constrainf(roundf(value * zoom2 * 100) / (zoom2 * 100), zoomMin / zoom2, zoomMax / zoom2); // ROUNDING TO CLOSEST PERCENT
}

void Sketch::drawContent()
{
  if (showImage)
  {
    imageState
      .glDisable(GL_BLEND)
      .setShaderMatrix(getTransform2D())
      .apply();
    backgroundBatch.flush();
  }

  // ---

  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(8, 1);

  if (showImage)
  {
    decalBatch.clear();

    drawingFill = false;
    drawingDecal = true;
    drawingGrid = false;
    drawGeometry();

    decalState
      .setShaderMatrix(getTransform3D())
      .apply();
    decalBatch.flush();
  }
  else
  {
    if (fillBatch.empty())
    {
      drawingFill = true;
      drawingDecal = false;
      drawingGrid = false;
      drawGeometry();
    }

    fillState
      .setShaderMatrix(getTransform3D())
      .apply();

    fillBatch.flush();
    maskBatch.flush();
  }

  glDepthMask(GL_FALSE);
  glDisable(GL_POLYGON_OFFSET_FILL);

  // ---

  if (!showImage)
  {
    if (gridBatch.empty())
    {
      drawingFill = false;
      drawingDecal = false;
      drawingGrid = true;
      drawGeometry();
    }

    gridState
      .setShaderMatrix(getTransform3D())
      .apply();
    gridBatch.flush();
  }

  // ---

  if (showImage)
  {
    imageState
      .glEnable(GL_BLEND)
      .setShaderMatrix(getTransform2D())
      .apply();
    foregroundBatch.flush();
  }
}

void Sketch::drawGeometry()
{
  float x, y, dx;
  Matrix matrix;

  // BOTTOM

  x = grid2.x - 50 * sy;
  y = grid2.y - 13 * sy;
  drawRect(matrix, x, y, grid2.x, y + 13 * sy, 0, 0);

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
  drawRect(matrix, 0, y, 40 * sy, y + (8 + 13 + 16 + 13 + 16) * sy, dx, 0);

  // MASKED

  x = -60 * sy;
  y = grid1.y - sy * (13 + 16);
  matrix.setTranslate(guideY1.x, 0, guideY1.z);
  drawRect(matrix, x, y, x + 60 * sy, y + 8 * sy, dx, 0);
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
  drawRing(matrix, 0, y, 13 * sy, radius3, PI, sy / 2, 0, 0);
  //
  y -= sy * (13 + 11.5f);
  drawRing(matrix, 0, y, 13 * sy, radius3, PI, sy / 2, 0, 0);
  //
  y -= sy * (13 + 16);
  drawRing(matrix, 0, y, 13 * sy, radius3, PI, sy / 2, 0, 0);
}

void Sketch::drawBalcon(float y)
{
  Matrix matrix;
  matrix.translate(0, 0, grid1.z - radius1);
  drawRing(matrix, grid1.x, y, 13 * sy, radius1, arc1, sy / 2, 0, 0);

  matrix.translate(0, 0, radius1);
  drawRect(matrix, grid1.x - 60 * sy, y, grid1.x, y + 13 * sy, 0, 0);

  if (!showImage && drawingFill)
  {
    matrix
      .translate(grid1.x, y + 13 * sy, 0)
      .rotateX(HALF_PI);

    draw::Rect()
      .setBounds(glm::vec2(-60 * sy, -radius1), glm::vec2(0, 0))
      .setFrontFace(CW)
      .append(maskBatch, matrix);

    draw::Circle()
      .setRadius(radius1)
      .setArc(0, arc1)
      .setSegmentLength(sy / 2)
      .setFrontFace(CW)
      .append(maskBatch, matrix, 0, -radius1);
  }
}

void Sketch::drawRect(const Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy)
{
  if (drawingFill)
  {
    rect.drawFill(fillBatch, matrix, x1, y1, x2 - x1, y2 - y1);
  }
  else if (drawingGrid)
  {
    rect.drawGrid(gridBatch, matrix, x1, y1, x2 - x1, y2 - y1, dx + ox, dy + oy);
  }
  else if (drawingDecal)
  {
    rect.drawDecal(decalBatch, matrix, x1, y1, x2 - x1, y2 - y1, dx + ox, dy + oy);
  }
}

void Sketch::drawRing(const Matrix &matrix, float x1, float y1, float h, float r, float a, float dd, float dx, float dy)
{
  if (drawingFill)
  {
    ring.drawFill(fillBatch, matrix, x1, y1, h, r, a, dd);
  }
  else if (drawingGrid)
  {
    ring.drawGrid(gridBatch, matrix, x1, y1, h, r, a, dd, dx + ox, dy + oy);
  }
  else if (drawingDecal)
  {
    ring.drawDecal(decalBatch, matrix, x1, y1, h, r, a, dd, dx + ox, dy + oy);
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
    Texture::ImageRequest("afro.png")
      .setMipmap(true)
      .setAnisotropy(true)
      .setWrap(GL_REPEAT, GL_REPEAT));
}

void Sketch::initImage()
{
  imageState
    .setShader(textureShader)
    .setShaderColor(1, 1, 1, 1)
    .glDisable(GL_DEPTH_TEST)
    .glDepthMask(GL_FALSE)
    .glDisable(GL_CULL_FACE);

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
    .glDepthMask(GL_FALSE)
    .glDisable(GL_CULL_FACE)
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 1);
}

void Sketch::drawFrame()
{
  auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

  frameState
    .setShaderMatrix(projectionMatrix)
    .apply();

  auto frame = getFrame();

  frameBatch.clear();

  if (frame.x1 > 0)
  {
    draw::Rect()
      .setBounds(0, 0, frame.x1, windowInfo.width)
      .append(frameBatch);
  }
  if (frame.x2 < windowInfo.width)
  {
    draw::Rect()
      .setBounds(frame.x2, 0, windowInfo.width, windowInfo.height)
      .append(frameBatch);
  }
  if (frame.y1 > 0)
  {
    draw::Rect()
      .setBounds(0, 0, windowInfo.width, frame.y1)
      .append(frameBatch);
  }
  if (frame.y2 < windowInfo.height)
  {
    draw::Rect()
      .setBounds(0, frame.y2, windowInfo.width, windowInfo.height)
      .append(frameBatch);
  }

  frameBatch.flush();
}

Rectf Sketch::getFrame() const
{
  auto size = foregroundTexture.innerSize * zoom * zoom2;
  auto p1 = (windowInfo.size - size) * 0.5f;
  auto p2 = p1 + size;

  return Rectf(p1, p2);
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
