#include "Sketch.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Circle.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

const float marginRatio = 12 / 768.0f; // RELATIVE TO SCREEN-HEIGHT
const float zoomMin = 0.5f;
const float zoomMax = 4.0f;
const float zNear = 1, zFar = 10000;

const float originalW = 1024; // WIDTH OF THE PICTURE ORIGINALLY USED FOR ESTIMATION
const float eyeZ = 2185.0964f;
const glm::mat4 rotation = {0.5031331f, 0.31332415f, 0.80540985f, 0.0f, -0.009767972f, -0.9298407f, 0.36783275f, 0.0f, -0.8641537f, 0.19293606f, 0.46477312f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

//

const glm::vec3 m1 = { 187.0f, +208.0f, 0.0f };
const glm::vec3 m2 = { -233.0f, +208.0f, 0.0f };
const glm::vec3 m3 = { -233.0f, -249.0f, 0.0f };
const glm::vec3 m4 = { 187.0f, -249.0f, 0.0f };
const float mx1 = m2.x;
const float my1 = m3.y;
const float mx5 = m1.x;
const float my5 = m1.y;
const float MX6 = 290 + 0 + 130; // BOX DEPTH
const float my2 = 91.0f;
const float mx2 = -45;
const float my3 = 9.0f;
const float mx3 = 41.0f;
const float my4 = 63.0f;
const float mz1 = m1.z;

const glm::vec3 m5 = { mx5, my1, mz1 - MX6 };
const glm::vec3 m6 = { mx1, my1, mz1 - MX6 };
const glm::vec3 m7 = { mx1, my5, mz1 - MX6 };
const glm::vec3 m8 = { mx5, my5, mz1 - MX6 };
const glm::vec3 m9 = { mx1, my2, mz1 };
const glm::vec3 m10 = { mx2, my2, mz1 };
const glm::vec3 m11 = { mx2, my3, mz1 };
const glm::vec3 m12 = { mx3, my3, mz1 };
const glm::vec3 m13 = { mx3, my4, mz1 };
const glm::vec3 m14 = { mx5, my4, mz1 };
const glm::vec3 m15 = { mx5, my4, mz1 - MX6 };
const glm::vec3 m16 = { mx2, my2, mz1 - MX6 };
const glm::vec3 m17 = { mx2, my3, mz1 - MX6 };
const glm::vec3 m18 = { mx3, my3, mz1 - MX6 };
const glm::vec3 m19 = { mx3, my4, mz1 - MX6 };
const glm::vec3 m20 = { mx3, my5, mz1 };
const glm::vec3 m21 = { mx3, my1, mz1 };
const glm::vec3 m22 = { mx1, my3, mz1 };
const glm::vec3 m23 = { mx3, my2, mz1 };
const glm::vec3 m24 = { mx1, my2, mz1 - MX6 };

//

const glm::vec3 pivot1 = { 187.0f, +208.0f, -169.0f };
const float angle1 = 3.8f;
const glm::vec3 pivot11 = { 111, 0, 0.0f };
const float angle11 = -2.39916f;
const glm::vec3 c1 = { 0.0f, 0.0f, 57.0f };
const glm::vec3 c2 = { 0.0f, -318.0f, 57.0f };
const glm::vec3 c3 = { 216.0f, -318.0f, 57.0f };
const glm::vec3 c4 = { 216.0f, 0.0f, 57.0f };
const float cx1 = c1.x;
const float cy1 = c2.y;
const float cy2 = -88.0f;
const float cx3 = c4.x;
const float cy3 = c1.y;
const float CX4 = 95 + 83 + 130; // BOX DEPTH
const float cz1 = c1.z;

const glm::vec3 c5 = { cx3, cy1, cz1 - CX4 };
const glm::vec3 c6 = { cx1, cy1, cz1 - CX4 };
const glm::vec3 c7 = { cx1, cy3, cz1 - CX4 };
const glm::vec3 c8 = { cx3, cy3, cz1 - CX4 };
const glm::vec3 c9 = { cx1, cy2, cz1 };
const glm::vec3 c10 = { cx3, cy2, cz1 };
const glm::vec3 c11 = { cx3, cy2, cz1 - CX4 };
const glm::vec3 c12 = { cx1, cy2, cz1 - CX4 };

//

const glm::vec3 pivot2= { -233.0f, +208.0f, -154.0f };
const float angle2 = 0.0f;
const glm::vec3 pivot22= { -113, 0, 0 };
const float angle22 = -0.6f;
const glm::vec3 f1 = { 0, 0, 343 };
const glm::vec3 f2 = { -418, 0, 343 };
const glm::vec3 f3 = { -418, -594, 343 };
const glm::vec3 f4 = { 0, -594, 343 };
const float fx1 = f2.x;
const float fy1 = f4.y;
const float fx4 = f1.x;
const float fy4 = f1.y;
const float FX7 = 409 + 70 + 130; // BOX DEPTH
const float fy2 = -131.0f;
const float fx2 = -200.0f;
const float fy3 = -220.0f;
const float fx5 = 223;
const float fy5 = -129;
const float fz1 = f1.z;

const glm::vec3 f5 = { fx4, fy1, fz1 - FX7 };
const glm::vec3 f6 = { fx1, fy1, fz1 - FX7 };
const glm::vec3 f7 = { fx1, fy4, fz1 - FX7 };
const glm::vec3 f8 = { fx4, fy4, fz1 - FX7 };
const glm::vec3 f9 = { fx1, fy2, fz1 };
const glm::vec3 f10 = { fx2, fy2, fz1 };
const glm::vec3 f16 = { fx2, fy4, fz1 };
const glm::vec3 f11 = { fx2, fy3, fz1 };
const glm::vec3 f12 = { fx4, fy3, fz1 };
const glm::vec3 f13 = { fx4, fy3, fz1 - fx5 };
const glm::vec3 f14 = { fx4, fy2, fz1 - fx5 };
const glm::vec3 f17 = { fx4, fy4, fz1 - fx5 };
const glm::vec3 f15 = { fx4, fy2, fz1 - FX7 };
const glm::vec3 f18 = { fx2, fy2, fz1 - fx5 };
const glm::vec3 f19 = { fx2, fy3, fz1 - fx5 };
const glm::vec3 f20 = { fx2, fy2, fz1 - FX7 };
const glm::vec3 f21 = { fx1, fy2, fz1 - FX7 };
const glm::vec3 f22 = { fx1, fy3, fz1 };
const glm::vec3 f23 = { fx4, fy3, fz1 - FX7 };

//

const glm::vec4 COLOR_FILL = { 0.075f, 0.075f, 0.075f, 1 };
const glm::vec4 COLOR_LINE = { 1, 1, 1, 0.1f };

void Sketch::setup()
{
  initButtonLayout();
  initTextures();

  // ---

  DecalProperties decalProperties[2];

  decalProperties[0].shader = textureAlphaShader;
  decalProperties[0].color = { 1, 1, 1, 0.175f };
  decalProperties[0].texture = decalTexture[0];
  decalProperties[0].zoom = 1;

  decalProperties[1].shader = textureAlphaShader;
  decalProperties[1].color = { 1, 1, 1, 0.175f };
  decalProperties[1].texture = decalTexture[1];
  decalProperties[1].zoom = 1;

  rect[1].setDecalProperties(decalProperties[0]);
  decalProperties[0].assign(decalState[0], decalBatch[0]);

  rect[3].setDecalProperties(decalProperties[1]);
  decalProperties[1].assign(decalState[1], decalBatch[1]);

  // ---

  fillBatch
    .setShader(colorShader);

  lineBatch
    .setPrimitive(GL_LINES)
    .setShader(colorShader);

  // ---

  GridProperties gridProperties[2];

  gridProperties[0].shader = colorShader;
  gridProperties[0].color = { 1, 1, 1, 0.175f };
  gridProperties[0].lineWidth = 2;
  gridProperties[0].gridSize = 256 / 18.0f;
  gridProperties[0].horizontal = true;
  gridProperties[0].vertical = false;

  gridProperties[1].shader = colorShader;
  gridProperties[1].color = { 1, 1, 1, 0.175f };
  gridProperties[1].lineWidth = 2;
  gridProperties[1].gridSize = 256 / 18.0f;
  gridProperties[1].horizontal = false;
  gridProperties[1].vertical = true;

  rect[0].setGridProperties(gridProperties[0]);
  gridProperties[0].assign(gridState, gridBatch);

  rect[2].setGridProperties(gridProperties[1]);
  gridProperties[1].assign(gridState, gridBatch);

  gridBatch.setPrimitive(GL_LINES);

  // ---

  initBackground();
  initFrame();

  backgroundState.glDisable(GL_BLEND);
  frameState.glDisable(GL_DEPTH_TEST);

  // ---

  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  zoom2 = originalW / backgroundTexture.innerWidth;
  setZoom((windowInfo.size.y - marginRatio * windowInfo.size.y * 2) / backgroundTexture.innerHeight / zoom2); // XXX

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

  float uf = fminf(1, t / 8);
  float um = fminf(1, t / 7);
  float uc = fminf(1, t / 6);

  cyf = 2 * (1 - ease(uf / 2 + 0.5f)) * 1185;
  cym = 2 * (1 - ease(um / 2 + 0.5f)) * 906;
  cyc = 2 * (1 - ease(uc / 2 + 0.5f)) * 635;

  if (t < 8)
  {
    showBackground = false;
    drawContent();
  }
  else if (t < 12)
  {
    auto frame = getFrame();
    float u = fminf(1, (t - 8) / 4);
    float clipX = (1 - u) * windowInfo.width;

    glEnable(GL_SCISSOR_TEST);

    glScissor(0, 0, (int)clipX, windowInfo.height);
    showBackground = false;
    drawContent();

    glScissor(int(clipX), 0, windowInfo.width, windowInfo.height);
    showBackground = true;
    drawContent();

    glDisable(GL_SCISSOR_TEST);
  }
  else
  {
    ox = (t - 12) * 20;

    showBackground = true;
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
    .translate(-backgroundTexture.innerSize * 0.5f);

  return modelViewMatrix * projectionMatrix;
}

glm::mat4 Sketch::getTransform3D()
{
  float halfHeight = zNear * windowInfo.height / (2.0 * eyeZ * zoom);
  float halfWidth = halfHeight * windowInfo.width / windowInfo.height;
  auto projectionMatrix = glm::frustum(-halfWidth, +halfWidth, -halfHeight, +halfHeight, zNear, zFar);

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

void Sketch::drawContent()
{
  if (showBackground)
  {
    backgroundState
      .setShaderMatrix(getTransform2D())
      .apply();
    backgroundBatch.flush();

    fillBatch.setShaderColor(0, 0, 0, 0);
    lineBatch.setShaderColor(0, 0, 0, 0);

    mx6 = 290;
    cx4 = 95;
    fx7 = 409;
  }
  else
  {
    fillBatch.setShaderColor(COLOR_FILL);
    lineBatch.setShaderColor(COLOR_LINE);

    mx6 = MX6;
    cx4 = CX4;
    fx7 = FX7;
  }

  Matrix matrix;

  // --- WALLS ---

  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(2, 5);

  fillBatch.clear();

  matrix
    .setTranslate(0, -cyf, 0);
  farWall(matrix, true);

  matrix
    .setIdentity()
    .setTranslate(0, -cym, 0);
  middleWall(matrix, true);

  matrix
    .setIdentity()
    .setTranslate(0, -cyc, 0);
  closeWall(matrix, true);

  matrix
    .setIdentity();
  farWall(matrix, false);
  middleWall(matrix, false);
  closeWall(matrix, false);

  State()
    .setShaderMatrix(getTransform3D())
    .apply();

  fillBatch.flush();

  glDepthMask(GL_FALSE);
  glDisable(GL_POLYGON_OFFSET_FILL);

  // --- MAPPING ---

  decalBatch[0].clear();
  decalBatch[1].clear();
  gridBatch.clear();
  lineBatch.clear();

  matrix
    .setIdentity()
    .translate(0, -cyf, 0);
  farMap(matrix, true);

  matrix
    .setIdentity()
    .translate(0, -cym, 0);
  middleMap(matrix, true);

  matrix
    .setIdentity()
    .translate(0, -cyc, 0);
  closeMap(matrix, true);

  matrix
    .setIdentity();
  middleMap(matrix, false);
  closeMap(matrix, false);
  farMap(matrix, false);

  decalState[0]
    .setShaderMatrix(getTransform3D())
    .apply();
  decalBatch[0].flush();

  decalState[1]
    .setShaderMatrix(getTransform3D())
    .apply();
  decalBatch[1].flush();

  gridState
    .setShaderMatrix(getTransform3D())
    .apply();
  gridBatch.flush();

  State()
    .glLineWidth(2)
    .setShaderMatrix(getTransform3D())
    .apply();
  lineBatch.flush();
}

void Sketch::middleWall(Matrix &matrix, bool up)
{
  if (up)
  {
    quad(matrix, m10, m9, m22, m11);
    quad(matrix, m12, m22, m3, m21);
    quad(matrix, m21, m4, m14, m13);
    quad(matrix, m14, m4, m5, m15);

    quad(matrix, m4, m3, m6, m5);
  }
  else
  {
    quad(matrix, m2, m9, m23, m20);
    quad(matrix, m10, m11, m12, m23);
    quad(matrix, m20, m13, m14, m1);
    quad(matrix, m1, m14, m15, m8);

    quadCCW(matrix, m1, m2, m7, m8);
  }

  quadCCW(matrix, m10, m9, m24, m16);
  quad(matrix, m10, m11, m17, m16);
  quadCCW(matrix, m11, m17, m18, m12);
  quad(matrix, m13, m12, m18, m19);
  quadCCW(matrix, m13, m19, m15, m14);
}

void Sketch::closeWall(Matrix &matrix, bool up)
{
  Matrix::Stack stack;

  matrix
    .push(stack)
    .translate(pivot1)
    .rotateY(angle1 * D2R);

  if (showBackground && !up)
  {
    maskQuad(matrix, 200, 0, 200, 100, 0, 100, 0, 0);
  }

  matrix
    .translate(pivot11)
    .rotateY(angle11 * D2R);

  if (up)
  {
    quad(matrix, c9, c2, c3, c10);
    quad(matrix, c10, c3, c5, c11);

    quad(matrix, c2, c6, c5, c3);
  }
  else
  {
    quad(matrix, c1, c9, c10, c4);
    quad(matrix, c4, c10, c11, c8);

    quadCCW(matrix, c1, c7, c8, c4);
  }

  quadCCW(matrix, c10, c9, c12, c11);

  matrix.pop(stack);
}

void Sketch::farWall(Matrix &matrix, bool up)
{
  Matrix::Stack stack;

  matrix
    .push(stack)
    .translate(pivot2)
    .rotateY(angle2 * D2R);

  if (showBackground && !up)
  {
    maskQuad(matrix, 10, 0, 10, 100, -150, 100, -150, 0);
  }

  matrix
    .translate(pivot22)
    .rotateY(angle22 * D2R);

  if (up)
  {
    quad(matrix, f10, f9, f22, f11);
    quad(matrix, f12, f22, f3, f4);
    quad(matrix, f12, f4, f5, f23);
    quad(matrix, f23, f15, f14, f13);

    quad(matrix, f3, f6, f5, f4);
  }
  else
  {
    quad(matrix, f16, f2, f9, f10);
    quad(matrix, f16, f11, f12, f1);
    quad(matrix, f1, f12, f13, f17);
    quad(matrix, f17, f14, f15, f8);

    quadCCW(matrix, f1, f2, f7, f8);
  }

  quadCCW(matrix, f10, f9, f21, f20);
  quadCCW(matrix, f18, f20, f15, f14);
  quadCCW(matrix, f11, f19, f13, f12);
  quad(matrix, f10, f11, f19, f18);
  quad(matrix, f18, f19, f13, f14);

  matrix.pop(stack);
}

void Sketch::middleMap(Matrix &matrix, bool up)
{
  Matrix::Stack stack;

  matrix
    .push(stack)
    .translate(0, 0, m2.z);

  if (up)
  {
    drawGrid(matrix, mx1, my1, mx2, my2, 0, 6);
    drawGrid(matrix, mx2, my1, mx5, my3, 0, 6);
    drawGrid(matrix, mx3, my3, mx5, my4, 0, 6);
  }
  else
  {
    drawDecal(matrix, mx1, my2, mx3, my5, 0, 0);
    drawDecal(matrix, mx2, my3, mx3, my2, 0, 0);
    drawDecal(matrix, mx3, my4, mx5, my5, 0, 0);
  }

  matrix
    .translate(mx5, 0, 0)
    .rotateY(HALF_PI);

  if (up)
  {
    drawGrid(matrix, 0, my1, mx6, my4, mx5, 6);
  }
  else
  {
    drawDecal(matrix, 0, my4, mx6, my5, mx5, 0);
  }

  matrix.pop(stack);

  // ---

  segment(matrix, m9, m10);
  segment(matrix, m10, m11);
  segment(matrix, m11, m12);
  segment(matrix, m12, m13);
  segment(matrix, m13, m14);
  segment(matrix, m14, m15);

  segment(matrix, m10, m16);
  segment(matrix, m11, m17);
  segment(matrix, m12, m18);
  segment(matrix, m13, m19);

  if (up)
  {
    segment(matrix, m3, m4);
    segment(matrix, m4, m5);
    segment(matrix, m9, m24);
    segment(matrix, m24, m16);
    segment(matrix, m16, m17);
    segment(matrix, m17, m18);
    segment(matrix, m19, m15);

    segment(matrix, m14, m4);
    segment(matrix, m9, m3);
    segment(matrix, m15, m5);
  }
  else
  {
    segment(matrix, m2, m1);
    segment(matrix, m1, m8);
    segment(matrix, m2, m7);
    segment(matrix, m7, m8);

    segment(matrix, m1, m14);
    segment(matrix, m2, m9);
    segment(matrix, m8, m15);
  }
}
void Sketch::closeMap(Matrix &matrix, bool up)
{
  Matrix::Stack stack;

  matrix
    .push(stack)
    .translate(pivot1)
    .rotateY(angle1 * D2R)
    .translate(pivot11)
    .rotateY(angle11 * D2R);

  matrix
    .push(stack)
    .translate(0, 0, c1.z);

  float dy = pivot1.y;

  if (up)
  {
    drawGrid(matrix, cx1, cy1, cx3, cy2, 0, dy + 8);
  }
  else
  {
    drawDecal(matrix, cx1, cy2, cx3, cy3, 0, dy);
  }

  matrix
    .translate(cx3 - cx1, 0, 0)
    .rotateY(HALF_PI);

  if (up)
  {
    drawGrid(matrix, 0, cy1, cx4, cy2, cx3 - cx1, dy + 8);
  }
  else
  {
    drawDecal(matrix, 0, cy2, cx4, cy3, cx3 - cx1, dy);
  }

  matrix.pop(stack);

  // ---

  segment(matrix, c9, c10);
  segment(matrix, c10, c11);

  if (up)
  {
    segment(matrix, c2, c3);
    segment(matrix, c3, c5);
    segment(matrix, c9, c12);
    segment(matrix, c12, c11);

    segment(matrix, c10, c3);
    segment(matrix, c9, c2);
    segment(matrix, c11, c5);
  }
  else
  {
    segment(matrix, c1, c4);
    segment(matrix, c4, c8);
    segment(matrix, c1, c7);
    segment(matrix, c7, c8);

    segment(matrix, c4, c10);
    segment(matrix, c1, c9);
    segment(matrix, c8, c11);
  }

  matrix.pop(stack);
}

void Sketch::farMap(Matrix &matrix, bool up)
{
  Matrix::Stack stack;

  matrix
    .push(stack)
    .translate(pivot2)
    .rotateY(angle2 * D2R)
    .translate(pivot22)
    .rotateY(angle2 * D2R);

  matrix
    .push(stack)
    .translate(0, 0, f2.z);

  float dy = pivot2.y;

  if (up)
  {
    drawGrid(matrix, fx1, fy1, fx2, fy2, 0, dy + 6);
    drawGrid(matrix, fx2, fy1, fx4, fy3, 0, dy + 6);
  }
  else
  {
    drawDecal(matrix, fx1, fy2, fx2, fy4, 0, dy);
    drawDecal(matrix, fx2, fy3, fx4, fy4, 0, dy);
  }

  matrix.rotateY(HALF_PI);

  if (up)
  {
    drawGrid(matrix, 0, fy1, fx5, fy3, fx4, dy + 6);
    drawGrid(matrix, fx5, fy1, fx7, fy5, fx4, dy + 6);
  }
  else
  {
    drawDecal(matrix, 0, fy3, fx5, fy4, fx4, dy);
    drawDecal(matrix, fx5, fy5, fx7, fy4, fx4, dy);
  }

  matrix.pop(stack);

  // ---

  segment(matrix, f9, f10); // XXX
  segment(matrix, f10, f11);
  segment(matrix, f11, f12);
  segment(matrix, f11, f19);
  segment(matrix, f19, f13);
  segment(matrix, f12, f13);
  segment(matrix, f13, f14);
  segment(matrix, f14, f15);
  segment(matrix, f10, f18);
  segment(matrix, f18, f14);
  segment(matrix, f18, f19);

  if (up)
  {
    segment(matrix, f3, f4); // XXX
    segment(matrix, f4, f5); // XXX
    segment(matrix, f9, f21);
    segment(matrix, f21, f15);

    segment(matrix, f12, f4);
    segment(matrix, f9, f3); // XXX
    segment(matrix, f15, f5);
  }
  else
  {
    segment(matrix, f2, f1);
    segment(matrix, f1, f8);
    segment(matrix, f2, f7);
    segment(matrix, f7, f8);

    segment(matrix, f1, f12);
    segment(matrix, f2, f9); // XXX
    segment(matrix, f8, f15);
  }

  matrix.pop(stack);
}

void Sketch::drawDecal(const Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy)
{
  auto &targetRect = showBackground ? rect[3] : rect[1];
  auto &targetBatch = showBackground ? decalBatch[1] : decalBatch[0];

  targetRect.drawDecal(targetBatch, matrix, x1, y1, x2 - x1, y2 - y1, ox + dx, oy + dy);
}

void Sketch::drawGrid(const Matrix &matrix, float x1, float y1, float x2, float y2, float dx, float dy)
{
  auto &targetRect = showBackground ? rect[2] : rect[0];
  targetRect.drawGrid(gridBatch, matrix, x1, y1, x2 - x1, y2 - y1, ox + x1 + dx, oy + y1 + dy);
}

void Sketch::maskQuad(const Matrix &matrix, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
  fillBatch
    .addVertex(matrix.transformPoint(x1, y1))
    .addVertex(matrix.transformPoint(x2, y2))
    .addVertex(matrix.transformPoint(x3, y3))
    .addVertex(matrix.transformPoint(x4, y4));

  fillBatch
    .addIndices(0, 3, 2, 2, 1, 0)
    .incrementIndices(4);}

void Sketch::quad(const Matrix &matrix, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &p4)
{
  fillBatch
    .addVertex(matrix.transformPoint(p1))
    .addVertex(matrix.transformPoint(p2))
    .addVertex(matrix.transformPoint(p3))
    .addVertex(matrix.transformPoint(p4));

  fillBatch
    .addIndices(0, 3, 2, 2, 1, 0)
    .incrementIndices(4);
}

void Sketch::quadCCW(const Matrix &matrix, const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &p4)
{
  fillBatch
    .addVertex(matrix.transformPoint(p1))
    .addVertex(matrix.transformPoint(p2))
    .addVertex(matrix.transformPoint(p3))
    .addVertex(matrix.transformPoint(p4));

  fillBatch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);
}

void Sketch::segment(const Matrix &matrix, const glm::vec3 &p1, const glm::vec3 &p2)
{
  lineBatch.addVertices(matrix.transformPoint(p1), matrix.transformPoint(p2));
}

void Sketch::initTextures()
{
  backgroundTexture = Texture(
    Texture::ImageRequest("286652175_1024.jpg")
      .setFlags(image::FLAGS_RBGA | image::FLAGS_POT));

  decalTexture[0] = Texture(
    Texture::ImageRequest("lys.png")
      .setFlags(image::FLAGS_TRANSLUCENT_INVERSE)
      .setMipmap(true)
      .setAnisotropy(true)
      .setWrap(GL_REPEAT, GL_REPEAT));

  decalTexture[1] = Texture(
    Texture::ImageRequest("lys.png")
      .setFlags(image::FLAGS_TRANSLUCENT)
      .setMipmap(true)
      .setAnisotropy(true)
      .setWrap(GL_REPEAT, GL_REPEAT));
}

void Sketch::initBackground()
{
  backgroundState
    .glDisable(GL_DEPTH_TEST)
    .glDepthMask(GL_FALSE)
    .glDisable(GL_CULL_FACE)
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
  auto size = backgroundTexture.innerSize * zoom * zoom2;
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
