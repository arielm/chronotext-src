#include "Episode1B.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace gl;

const float LINE_H = 8;
const float TERRAIN_H = 150;
const float GRID_SIZE = 4;

const float FILL_POLYGON_OFFSET = 8.0f;
const float TEXT_POLYGON_OFFSET = 0.25f;

const float R1 = 0;
const float R2 = 75;
const float TURNS = 36;
const float H = 300;
const float DD1 = 1;
const float DD2 = 9;

const float BORDER_HM = 640;
const float BORDER_HP = 640;
const float BORDER_VM = 216;
const float BORDER_VP = 512;

const glm::vec3 COLOR_BACK(0.9f, 0.9f, 0.9f);
const glm::vec4 COLOR_SOIL(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_TEXT_CAMPS(0.0f, 0.0f, 0.0f, 0.667f);
const glm::vec4 COLOR_TEXT_SOIL(COLOR_TEXT_CAMPS.r, COLOR_TEXT_CAMPS.g, COLOR_TEXT_CAMPS.b, 0.333f);
const glm::vec4 COLOR_WIRE_SOIL(COLOR_TEXT_CAMPS.r, COLOR_TEXT_CAMPS.g, COLOR_TEXT_CAMPS.b, 0.2f);
const glm::vec4 COLOR_COLUMN_TEXT_DARK(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 COLOR_COLUMN_TEXT_LIGHT(COLOR_COLUMN_TEXT_DARK.r, COLOR_COLUMN_TEXT_DARK.g, COLOR_COLUMN_TEXT_DARK.b, 0.333f);
const glm::vec4 COLOR_COLUMN_WIRE(COLOR_COLUMN_TEXT_DARK.r, COLOR_COLUMN_TEXT_DARK.g, COLOR_COLUMN_TEXT_DARK.b, 0.2f);

const float OX_TURN[] = {672, 468};
const double END_TURN[] = {36, 30};

Episode1B::Episode1B(const Episode::Context &context, Episode::Lang lang)
:
Episode(context, lang),
fogManager(context.clock, FogManager::MODE_EXP2),
fadeManager(context.clock),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag")),
surface(800, GRID_SIZE, 0.01f, 3, 123456789),
viewOY(0),
textOX(0),
textOY(0),
turn(0)
{}

void Episode1B::setup()
{
  switch (lang)
  {
    case LANG_HE:
      fontSize = 8;
      fontSize1 = 2;
      fontSize2 = 24;
      textDir = -1;
      mirror = false;

      font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;

    case LANG_EN:
    case LANG_FR:
      fontSize = 6;
      fontSize1 = 1.5f;
      fontSize2 = 18;
      textDir = +1;
      mirror = true;

      font = fontManager.getFont(InputSource::resource("Georgia_Bold_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;
  }

  font->setDirection(textDir);

  switch (lang)
  {
    case LANG_HE:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Numbers_02.txt"));
      line = utils::readText<u16string>(InputSource::resource("Exodus_25_8.txt"));
      break;

    case LANG_EN:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Numbers_02_EN.txt"));
      line = utils::readText<u16string>(InputSource::resource("Exodus_25_8_EN.txt"));
      break;

    case LANG_FR:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Numbers_02_FR.txt"));
      line = utils::readText<u16string>(InputSource::resource("Exodus_25_8_FR.txt"));
      break;
  }

  // ---

  surface.generateBase();

  patch = TextNoisePatch(&surface, TEXT_POLYGON_OFFSET, font, lineProvider, LINE_H);
  patch.setup(-BORDER_HM, -BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H);
  patch.update(TERRAIN_H, glm::vec2(0));

  font->setAxis(mirror ? -1 : +1, -1);

  font->beginSequence(sequenceSoil);
  patch.drawText(-BORDER_HM, -BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, textOX, textOY, TERRAIN_H);
  font->endSequence();

  // ---

  helix = Helix(font);
  helix.update(R1, R2, TURNS, H, DD1, DD2);

  font->setAxis(mirror ? + 1 : -1, +1);

  font->beginSequence(sequenceColumn);
  helix.drawText(line, R1, R2, TURNS, H, 0, fontSize1, fontSize2);
  font->endSequence();

  // ---

  view = View(clock, 60 * D2R, 1, 2000, mirror);
  view.resize(windowSize); // XXX
  view.add(glm::quat(0.8291919f, -0.5585135f, -0.002259369f, -0.022322547f), 291.0f, glm::vec3(0, 0, 0));
  view.add(glm::quat(0.8291919f, -0.47997266f, -0.019931437f, -0.037154738f), 289.6949f, glm::vec3(0, 0, 0));
  view.add(glm::quat(0.93518215f, -0.35395208f, -0.012192054f, 0.0019165052f), 292.0f, glm::vec3(-75, 59, 0));
  view.add(glm::quat(0.9960813f, -0.021123046f, -0.032921277f, 0.07932285f), 240.0f, glm::vec3(0, -10, 0));

  fogManager.add(COLOR_BACK, 0.025f);
  fogManager.add(COLOR_BACK, 0.0125f);
  fogManager.add(COLOR_BACK, 0.002f);

  fadeManager.resize(windowSize); // XXX
}

void Episode1B::enter()
{
  switch (turn)
  {
    case 0 :
      view.interpolate(0, 1, 38, true);
      fogManager.interpolate(0, 2, 4);
      step = 1;
      break;

    case 1 :
      view.set(2);
      fogManager.interpolate(1, 2, 4);
      step = 0;
      break;
  }

  fadeManager.reset();

  // ---

  font->setShader(fogTextureAlphaShader);
}

void Episode1B::leave()
{
  turn = (turn + 1) % 2;
}

void Episode1B::update()
{
  switch (step)
  {
    case 0 :
      if (clock->getTime() >= 24)
      {
        view.set(3);
        step = 1;
        break;
      }

    case 1 :
      if (clock->getTime() >= END_TURN[turn])
      {
        fadeManager.interpolate(glm::vec3(0, 0, 0), FadeManager::FADE_OUT, 2);
        step = 2;
      }
      break;

    case 2 :
      if (clock->getTime() >= END_TURN[turn] + 4)
      {
        nextEpisode();
      }
      break;
  }

  float d = 672 * clock->getTime() / 38;
  viewOX = OX_TURN[turn] - d;
  azimuth = d / R2; // RADIANS
}

void Episode1B::draw()
{
  glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
  glDepthMask(true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  glm::mat4x4 projectionMatrix;
  Matrix modelViewMatrix;
  tie(projectionMatrix, modelViewMatrix) = view.run();

  modelViewMatrix.translate(0, 0, -(surface.noiseMin + (surface.noiseMax - surface.noiseMin) * 0.5f) * TERRAIN_H);

  State state;
  state
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix);

  fogManager.run(state);
  state.apply();

  // ---

  glEnable(GL_CULL_FACE);
  glFrontFace(mirror ? GL_CCW : GL_CW);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(true);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(FILL_POLYGON_OFFSET, 1.0f);

  patch.drawFill(COLOR_SOIL);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDepthMask(false);

  patch.drawGrid(COLOR_WIRE_SOIL);

  // ---

  font->setColor(COLOR_TEXT_SOIL);
  font->replaySequence(sequenceSoil);

  font->setColor(COLOR_TEXT_CAMPS);
  drawCamps(viewOX, viewOY, 72, 16);

  // ---

  modelViewMatrix
    .translate(viewOX, viewOY, TERRAIN_H * surface.getHeight(viewOX, viewOY))
    .rotateZ(azimuth);

  state
    .setShaderMatrix(modelViewMatrix * projectionMatrix)
    .apply();

  helix.wireBatch
    .setPrimitive(GL_LINE_STRIP)
    .setShaderColor(COLOR_COLUMN_WIRE)
    .flush();

  font->setColor(COLOR_COLUMN_TEXT_DARK);
  font->replaySequence(sequenceColumn);

  glFrontFace(mirror ? GL_CW : GL_CCW);

  font->setColor(COLOR_COLUMN_TEXT_LIGHT);
  font->replaySequence(sequenceColumn);

  // ---

  fadeManager.run();
}

void Episode1B::drawCamps(float x, float y, float d2, float d3)
{
  font->setSize(fontSize);
  font->setAxis(mirror ? -1 : +1, -1);

  font->beginSequence(sequenceCamps);

  patch.drawText(x + d2 * -1.5f - d3, y + d2 * 1.5f + d3, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * -0.5f, y + d2 * 1.5f + d3, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * +0.5f + d3, y + d2 * 1.5f + d3, d2, d2, textOX, textOY, TERRAIN_H);

  patch.drawText(x + d2 * -1.5f - d3, y + d2 * -2.5f - d3, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * -0.5f, y + d2 * -2.5f - d3, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * +0.5f + d3, y + d2 * -2.5f - d3, d2, d2, textOX, textOY, TERRAIN_H);

  patch.drawText(x + d2 * -2.5f - d3 * 2.0f, y + d2 * -1.5f - d3, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * -2.5f - d3 * 2.0f, y + d2 * -0.5f, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * -2.5f - d3 * 2.0f, y + d2 * +0.5f + d3, d2, d2, textOX, textOY, TERRAIN_H);

  patch.drawText(x + d2 * +1.5f + d3 * 2.0f, y + d2 * -1.5f - d3, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * +1.5f + d3 * 2.0f, y + d2 * -0.5f, d2, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * +1.5f + d3 * 2.0f, y + d2 * +0.5f + d3, d2, d2, textOX, textOY, TERRAIN_H);

  // ---

  patch.drawText(x + d2 * -0.5f, y + d2 * +0.5f, d2, d3, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * -0.5f - d3, y + d2 * -0.5f, d3, d2, textOX, textOY, TERRAIN_H);
  patch.drawText(x + d2 * -0.5f, y + d2 * -0.5f - d3, d2, d3, textOX, textOY, TERRAIN_H);

  font->endSequence();
  font->replaySequence(sequenceCamps);
}
