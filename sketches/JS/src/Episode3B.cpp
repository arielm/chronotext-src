#include "Episode3B.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace gl;

const float LINE_H = 4;
const float TERRAIN_H = 150;
const float GRID_SIZE = 2;

const float FILL_POLYGON_OFFSET = 8.0f;
const float TEXT_POLYGON_OFFSET = 0.125f;

const float BORDER_HM = 100;
const float BORDER_HP = 300;
const float BORDER_VM = 100;
const float BORDER_VP = 120;

const glm::vec3 COLOR_BACK(1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_TEXT(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 COLOR_WIRE(COLOR_TEXT.r, COLOR_TEXT.g, COLOR_TEXT.b, 0.2f);
const float GRAY_SEA = 0.4f;

Episode3B::Episode3B(const Episode::Context &context, Episode::Lang lang)
:
Episode(context, lang),
fogManager(context.clock, FogManager::MODE_EXP),
fadeManager(context.clock),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag")),
noiseSurface(400, GRID_SIZE, 0.01f, 4, 123456789),
seaPatch(&seaSurface),
viewOX(300),
viewOY(0)
{}

void Episode3B::setup()
{
  switch (lang)
  {
    case LANG_HE:
      fontSize = 4;
      textDir = -1;
      mirror = false;

      font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;

    case LANG_EN:
    case LANG_FR:
      fontSize = 3;
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
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Ezekiel_47.txt"));
      break;

    case LANG_EN:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Ezekiel_47_EN.txt"));
      break;

    case LANG_FR:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("Ezekiel_47_FR.txt"));
      break;
  }

  // ---

  noiseSurface.setBaseFunction(bind(&Episode3B::cliffNoiseBase, this, std::placeholders::_1, std::placeholders::_2));
  noiseSurface.generateBase();

  noisePatch = TextNoisePatch(&noiseSurface, TEXT_POLYGON_OFFSET, font, lineProvider, LINE_H);
  noisePatch.setup(viewOX - BORDER_HM, viewOY - BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H | NoisePatch::MODE_GRID_V);
  noisePatch.update(TERRAIN_H, glm::vec2(0));

  // ---

  seaPatch.setup(180, 0, 440, 108, 4, SeaPatch::MODE_FILL),
  seaSpiral.setup(seaSurface, 600, 0, 1, 800, 100, 4, 8, GRAY_SEA);

  // ---

  view = View(clock, 60 * D2R, 1, 2000, mirror);
  view.resize(windowSize); // XXX
  view.add(glm::quat(0.89284503f, 0.02953783f, 0.44696882f, 0.04662688f), 131, glm::vec3(0, 0, 0));

  fogManager.add(COLOR_BACK, 0.1f);
  fogManager.add(COLOR_BACK, 0);

  fadeManager.resize(windowSize); // XXX
}

void Episode3B::enter()
{
  step = 0;

  view.set(0);
  fogManager.interpolate(0, 1, 4);
  fadeManager.reset();

  // ---

  font->setShader(fogTextureAlphaShader);
}

void Episode3B::update()
{
  switch (step)
  {
    case 0 :
      if (clock->getTime() >= 4)
      {
        step = 1;
      }
      break;

    case 1 :
      if (clock->getTime() >= 20)
      {
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_OUT, 2);
        step = 2;
      }
      break;

    case 2 :
      if (clock->getTime() >= 24)
      {
        nextEpisode();
      }
      break;
  }

  if (step == 0)
  {
    textOX = 80;
    level = 100;
  }
  else
  {
    float ratio = math::ease(fminf((clock->getTime() - 4) / 20, 1));
    textOX = 80 - ratio * 120;
    level = 100 - ratio * 108;
  }

  seaPatch.update(clock->getTime(), 1);
  seaSpiral.update(seaSurface, clock->getTime(), 1);
}

void Episode3B::draw()
{
  glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
  glDepthMask(true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  glm::mat4x4 projectionMatrix;
  Matrix modelViewMatrix;
  tie(projectionMatrix, modelViewMatrix) = view.run();

  modelViewMatrix.translate(-viewOX, -viewOY);

  State state;
  state
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix);

  fogManager.run(state);
  state.apply();

  glEnable(GL_CULL_FACE);
  glFrontFace(mirror ? GL_CCW : GL_CW);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(true);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(FILL_POLYGON_OFFSET, 1.0f);

  noisePatch.drawFill(glm::vec4(1, 1, 1, 1));

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDepthMask(false);

  noisePatch.drawGrid(COLOR_WIRE);

  //

  font->setColor(COLOR_TEXT);
  font->setSize(fontSize);

  font->beginSequence(sequence);
  noisePatch.drawText(viewOX - BORDER_HM, viewOY - BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, textOX, textOY, TERRAIN_H);
  font->endSequence();
  font->replaySequence(sequence);

  // ---

  modelViewMatrix
    .rotateX(90 * D2R)
    .translate(0, 0, level);

  state
    .setShaderMatrix(modelViewMatrix * projectionMatrix)
    .apply();

  glFrontFace(mirror ? GL_CW : GL_CCW);

  seaPatch.drawFill(glm::vec4(1, 1, 1, 1));
  seaSpiral.draw(fogColorShader);

  // ---

  fadeManager.run();
}

float Episode3B::cliffNoiseBase(float x, float y)
{
  return posterize(noiseSurface.tilingNoise1d(y, 0.05f), 0.1f) * noiseSurface.tilingNoise2d(x, y, noiseSurface.noiseScale);
}

float Episode3B::posterize(float x, float factor)
{
  return floorf(x / factor) * factor;
}
