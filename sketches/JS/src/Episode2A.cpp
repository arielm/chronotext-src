#include "Episode2A.h"

using namespace std;
using namespace chr;
using namespace gl;

const float S = 4;
const float D = 36;

const float LINE_SOIL_H = 8;
const float LINE_GATE_H = 8;

const float TERRAIN_H = 150;
const float GRID_SIZE = 4;
const float ARC_DD = GRID_SIZE;

const float FILL_POLYGON_OFFSET = 8.0f;
const float TEXT_POLYGON_OFFSET = 0.25f;

const float BORDER_HM = 1200;
const float BORDER_HP = 0;
const float BORDER_VM = 480;
const float BORDER_VP = 720;

const glm::vec3 COLOR_BACK(0.2f, 0.2f, 0.2f);
const glm::vec4 COLOR_SOIL(0.333f, 0.333f, 0.333f, 1.0f);
const glm::vec4 COLOR_SOIL_TEXT_LIGHT(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_SOIL_TEXT_DARK(COLOR_SOIL_TEXT_LIGHT.r, COLOR_SOIL_TEXT_LIGHT.g, COLOR_SOIL_TEXT_LIGHT.b, 0.5f);
const glm::vec4 COLOR_SOIL_WIRE(COLOR_SOIL_TEXT_LIGHT.r, COLOR_SOIL_TEXT_LIGHT.g, COLOR_SOIL_TEXT_LIGHT.b, 0.125f);
const glm::vec4 COLOR_GATE(0.333f, 0.333f, 0.333f, 1.0f);
const glm::vec4 COLOR_GATE_TEXT(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_GATE_WIRE(COLOR_GATE_TEXT.r, COLOR_GATE_TEXT.g, COLOR_GATE_TEXT.b, 0.2f);

Episode2A::Episode2A(const Episode::Context &context, Episode::Lang lang)
:
Episode(context, lang),
fogManager(context.clock, FogManager::MODE_EXP2),
fadeManager(context.clock),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag")),
surface(800, GRID_SIZE, 0.01f, 2, 123456789),
textSoilOY(0),
textGateOY(0),
turn(0)
{}

void Episode2A::setup()
{
  switch (lang)
  {
    case LANG_HE:
      fontSoilSize = 8;
      fontGateSize = 8;
      textDir = -1;
      mirror = false;

      font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;

    case LANG_EN:
    case LANG_FR:
      fontSoilSize = 6;
      fontGateSize = 6;
      textDir = +1;
      mirror = true;

      font = fontManager.getFont(InputSource::resource("Georgia_Bold_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;
  }

  font->setDirection(textDir);
  font->setAxis(mirror ? -1 : +1, -1);

  switch (lang)
  {
    case LANG_HE:
      lineProviderSoil = make_shared<LineProvider>(*font, fontSoilSize, InputSource::resource("Isaiah_40.txt"));
      lineProviderGate = make_shared<LineProvider>(*font, fontGateSize, InputSource::resource("Ezra_01.txt"));
      break;

    case LANG_EN:
      lineProviderSoil = make_shared<LineProvider>(*font, fontSoilSize, InputSource::resource("Isaiah_40_EN.txt"));
      lineProviderGate = make_shared<LineProvider>(*font, fontGateSize, InputSource::resource("Ezra_01_EN.txt"));
      break;

    case LANG_FR:
      lineProviderSoil = make_shared<LineProvider>(*font, fontSoilSize, InputSource::resource("Isaiah_40_FR.txt"));
      lineProviderGate = make_shared<LineProvider>(*font, fontGateSize, InputSource::resource("Ezra_01_FR.txt"));
      break;
  }

  // ---

  surface.generateBase();

  patch = TextNoisePatch(&surface, TEXT_POLYGON_OFFSET, font, lineProviderSoil, LINE_SOIL_H);
  patch.setup(-BORDER_HM, -BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H);
  patch.update(TERRAIN_H, glm::vec2(0));

  font->setSize(fontSoilSize);

  float xm = -BORDER_HM;
  float xp = +BORDER_HP;
  float ym = -BORDER_VM;
  float yp = +BORDER_VP;

  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);
  float minus = ym + (baselineOffset + font->getAscent()) - fmodf(textSoilOY, LINE_SOIL_H);
  float plus = yp - (baselineOffset - font->getDescent()) - fmodf(textSoilOY, LINE_SOIL_H);

  int total_m = int(minus / LINE_SOIL_H);
  int total_p = int(plus / LINE_SOIL_H);

  font->beginSequence(sequenceSoil1);

  for (int i = total_m; i <= total_p; i++)
  {
    if (i != 0)
    {
      patch.drawLine(i, xm, xp, 0, textSoilOY, TERRAIN_H);
    }
  }

  font->endSequence();

  // ---

  rectangle1 = TextRectangle(font, lineProviderGate, LINE_GATE_H);
  rectangle1.matrix
    .setTranslate(-28 * S, 0, 0)
    .rotateY(+90 * D2R);
  rectangle1.update(0, 0, D * S, 84 * S, GRID_SIZE);

  rectangle2 = TextRectangle(font, lineProviderGate, LINE_GATE_H);
  rectangle2.matrix
    .setTranslate(+28 * S, 0, 0)
    .rotateY(-90 * D2R);
  rectangle2.update(-D * S, 0, D * S, 84 * S, GRID_SIZE);

  arc1 = TextArc(font, lineProviderGate, LINE_GATE_H);
  arc1.matrix
    .setTranslate(0, 84 * S, -15 * S)
    .rotateY(+90 * D2R);
  arc1.update(-15 * S, 0, D * S, 28 * S, HALF_PI, GRID_SIZE, ARC_DD);

  arc2 = TextArc(font, lineProviderGate, LINE_GATE_H);
  arc2.matrix
    .setTranslate(0, 84 * S, -15 * S)
    .rotateY(-90 * D2R);
  arc2.update((-D + 15) * S, 0, D * S, 28 * S, HALF_PI, GRID_SIZE, ARC_DD);

  // ---

  view = View(clock, 60 * D2R, 1, 2000, mirror);
  view.resize(windowSize); // XXX
  view.add(glm::quat(0.17047925f, 0.0f, -0.9853613f, 0.0f), 230, glm::vec3(10, -319, 0)); // 1A
  view.add(glm::quat(0.17047925f, 0.0f, -0.9853613f, 0.0f), 187.0f, glm::vec3(10, -319, 0)); // 2A
  view.add(glm::quat(0.41256776f, -0.008796372f, -0.9107284f, 0.016861191f), 165, glm::vec3(13, -299, 0)); // 1C
  view.add(glm::quat(0.41256776f, -0.008796372f, -0.9107284f, 0.016861191f), 113, glm::vec3(13, -271, 0)); // 2C

  fogManager.add(COLOR_BACK, 0.002f);
  fogManager.add(COLOR_BACK, 0.001f);

  fadeManager.resize(windowSize); // XXX
}

void Episode2A::enter()
{
  step = 0;

  switch (turn)
  {
    case 0 :
      view.interpolate(0, 1, 23.35);
      break;

    case 1 :
      view.interpolate(2, 3, 24);
      break;
  }

  fadeManager.reset();
  fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_IN, 2);
}

void Episode2A::leave()
{
  turn = (turn + 1) % 2;
}

void Episode2A::update()
{
  switch (step)
  {
    case 0 :
      if (clock->getTime() >= 21)
      {
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_OUT, 2);
        step = 1;
      }
      break;

    case 1 :
      if (clock->getTime() >= 24)
      {
        nextEpisode();
      }
      break;
  }

  textGateOX = -3 * clock->getTime();
  textSoilOX = +3 * clock->getTime();
}

void Episode2A::draw()
{
  glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
  glDepthMask(true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  glm::mat4x4 projectionMatrix;
  Matrix modelViewMatrix;
  tie(projectionMatrix, modelViewMatrix) = view.run();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glEnable(GL_CULL_FACE);
  glFrontFace(mirror ? GL_CCW : GL_CW);

  drawSoil(modelViewMatrix, projectionMatrix);
  drawGate(modelViewMatrix, projectionMatrix);

  // ---

  fadeManager.run();
}

void Episode2A::drawSoil(Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix)
{
  Matrix::Stack stack;

  modelViewMatrix
    .push(stack)
    .rotateX(-90 * D2R)
    .rotateZ(+90 * D2R)
    .translate(50, 0, 125);

  State state;
  state
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix);

  fogManager.set(0);
  fogManager.run(state);
  state.apply();

  modelViewMatrix.pop(stack);

  glDepthMask(true);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(FILL_POLYGON_OFFSET, 1.0f);

  patch.drawFill(COLOR_SOIL);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDepthMask(false);

  patch.drawGrid(COLOR_SOIL_WIRE);

  // ---

  font->setColor(COLOR_SOIL_TEXT_DARK);
  font->setShader(fogTextureAlphaShader);
  font->replaySequence(sequenceSoil1);

  // ---

  fogManager.set(1);
  fogManager.run(state);
  state.apply();

  font->setColor(COLOR_SOIL_TEXT_LIGHT);
  font->setSize(fontSoilSize);

  font->beginSequence(sequenceSoil2);

  patch.drawLine(0, -BORDER_HM, +BORDER_HP, textSoilOX, textSoilOY, TERRAIN_H);

  font->endSequence();
  font->replaySequence(sequenceSoil2);
}

void Episode2A::drawGate(Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix)
{
  State()
    .setShader(colorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix)
    .apply();

  glDepthMask(true);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.5f, 1);

  rectangle1.drawFill(COLOR_GATE);
  rectangle2.drawFill(COLOR_GATE);
  arc1.drawFill(COLOR_GATE);
  arc2.drawFill(COLOR_GATE);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDepthMask(false);

  rectangle1.drawGrid(COLOR_GATE_WIRE);
  rectangle2.drawGrid(COLOR_GATE_WIRE);
  arc1.drawGrid(COLOR_GATE_WIRE);
  arc2.drawGrid(COLOR_GATE_WIRE);

  // ---

  font->setColor(COLOR_GATE_TEXT);
  font->setSize(fontGateSize);

  font->setShader(textureAlphaShader);
  font->beginSequence(sequenceGate);

  rectangle1.drawText(0, 0, D * S, 84 * S, textGateOX - 28 * S, textGateOY);
  rectangle2.drawText(-D * S, 0, D * S, 84 * S, textGateOX + 28 * S, textGateOY);

  arc1.drawText(-15 * S, 0, D * S, 28 * S, HALF_PI, textGateOX - 13 * S, textGateOY - 84 * S);
  arc2.drawText((-D + 15) * S, 0, D * S, 28 * S, HALF_PI, textGateOX + 13 * S, textGateOY - 84 * S);

  font->endSequence();
  font->replaySequence(sequenceGate);
}
