#include "Episode2B.h"

using namespace std;
using namespace chr;
using namespace gl;

const float LINE_H = 8;
const float TERRAIN_H = 150;
const float GRID_SIZE = 4;

const float FILL_POLYGON_OFFSET = 8.0f;
const float TEXT_POLYGON_OFFSET = 0.25f;

const float BORDER_HM = 256;
const float BORDER_HP = 144;
const float BORDER_VM = 96;
const float BORDER_VP = 160;

const glm::vec3 COLOR_BACK(0.0f, 0.0f, 0.0f);
const glm::vec4 COLOR_SOIL(0.5f, 0.5f, 0.5f, 1.0f);
const glm::vec4 COLOR_TEXT(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_WIRE(COLOR_TEXT.r, COLOR_TEXT.g, COLOR_TEXT.b, 0.5f);

Episode2B::Episode2B(const Episode::Context &context, Episode::Lang lang)
:
Episode(context, lang),
fogManager(context.clock, FogManager::MODE_EXP2),
fadeManager(context.clock),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag")),
surface(400, GRID_SIZE, 0.01f, 3, 123456789),
viewOY(0),
textOY(0)
{}

void Episode2B::setup()
{
  switch (lang)
  {
    case LANG_HE:
      fontSize = 8;
      textDir = -1;
      mirror = false;

      font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;

    case LANG_EN:
    case LANG_FR:
      fontSize = 6;
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
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Isaiah_40.txt"));
      break;

    case LANG_EN:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Isaiah_40_EN.txt"));
      break;

    case LANG_FR:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Isaiah_40_FR.txt"));
      break;
  }

  // ---

  surface.generateBase();
  patch = TextNoisePatch(&surface, TEXT_POLYGON_OFFSET, font, lineProvider, LINE_H);

  // ---

  view = View(clock, 45 * D2R, 1, 2000, mirror);
  view.resize(windowSize); // XXX
  view.add(glm::quat(0.9312818f, -0.34581113f, -0.073816165f, -0.087636344f), 152, glm::vec3(0, 0, 0));

  fogManager.add(COLOR_BACK, 0.0175f);
  fogManager.add(COLOR_BACK, 0.00667f);

  fadeManager.resize(windowSize); // XXX
}

void Episode2B::enter()
{
  step = 0;

  view.set(0);
  fadeManager.reset();
  fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_IN, 0.25f);
  fogManager.interpolate(0, 1, 4);
}

void Episode2B::update()
{
  switch (step)
  {
    case 0 :
      if (clock->getTime() >= 26)
      {
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_OUT, 2);
        step = 1;
      }
      break;

    case 1 :
      if (clock->getTime() >= 30)
      {
        nextEpisode();
      }
      break;
  }

  viewOX = -30 * clock->getTime();
  textOX = -5 * clock->getTime();
  terrainH = fmaxf(TERRAIN_H - 6 * clock->getTime(), 0);
}

void Episode2B::draw()
{
  glDepthMask(true);
  glClear(GL_DEPTH_BUFFER_BIT);

  // ---

  glm::mat4x4 projectionMatrix;
  Matrix modelViewMatrix;
  tie(projectionMatrix, modelViewMatrix) = view.run();

  modelViewMatrix.translate(-viewOX, -viewOY, -(surface.noiseMin + (surface.noiseMax - surface.noiseMin) * 0.5f) * terrainH);

  State state;
  state
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix);

  fogManager.run(state);
  state.apply();

  // ---

  patch.setup(viewOX - BORDER_HM, viewOY - BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H);
  patch.update(terrainH, glm::vec2(0));

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

  patch.drawGrid(COLOR_WIRE);

  // ---

  float xm = viewOX - BORDER_HM;
  float xp = viewOX + BORDER_HP;
  float ym = viewOY - BORDER_VM;
  float yp = viewOY + BORDER_VP;

  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);
  float minus = ym + (baselineOffset + font->getAscent()) - fmodf(textOY, LINE_H);
  float plus = yp - (baselineOffset - font->getDescent()) - fmodf(textOY, LINE_H);

  int total_m = int(minus / LINE_H);
  int total_p = int(plus / LINE_H);

  font->setColor(COLOR_TEXT);
  font->setShader(fogTextureAlphaShader);
  font->beginSequence(sequence);

  for (int i = total_m; i <= total_p; i++)
  {
    if (i != 0)
    {
      patch.drawLine(i, xm, xp, 0, textOY, terrainH);
    }
  }

  font->endSequence();
  font->replaySequence(sequence);

  //

  font->setShader(textureAlphaShader);
  font->beginSequence(sequence);

  patch.drawLine(0, xm, xp, textOX, textOY, terrainH);

  font->endSequence();
  font->replaySequence(sequence);

  // ---

  fadeManager.run();
}
