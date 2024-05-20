#include "Episode1A.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace gl;

const float R1 = 0;
const float R2 = 50;
const float TURNS = 16;
const float H = 200;
const float DD1 = 0.125f;
const float DD2 = 4;

const glm::vec3 COLOR_BACK(1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_COLUMN_TEXT_DARK(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 COLOR_COLUMN_TEXT_LIGHT(COLOR_COLUMN_TEXT_DARK.r, COLOR_COLUMN_TEXT_DARK.g, COLOR_COLUMN_TEXT_DARK.b, 0.333f);
const glm::vec4 COLOR_COLUMN_WIRE(COLOR_COLUMN_TEXT_DARK.r, COLOR_COLUMN_TEXT_DARK.g, COLOR_COLUMN_TEXT_DARK.b, 0.2f);
const float GRAY_SEA_1 = 0.25f;
const float GRAY_SEA_2 = 0.4f;

Episode1A::Episode1A(const Episode::Context &context, Episode::Lang lang)
:
Episode(context, lang),
fogManager(context.clock, FogManager::MODE_EXP),
fadeManager(context.clock),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag"))
{}

void Episode1A::setup()
{
  switch (lang)
  {
    case LANG_HE:
      fontSize1 = 2;
      fontSize2 = 16;
      textDir = -1;
      mirror = false;

      font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;

    case LANG_EN:
    case LANG_FR:
      fontSize1 = 1.5f;
      fontSize2 = 12;
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
      line = utils::readText<u16string>(InputSource::resource("Exodus_25_8.txt"));
      break;

    case LANG_EN:
      line = utils::readText<u16string>(InputSource::resource("Exodus_25_8_EN.txt"));
      break;

    case LANG_FR:
      line = utils::readText<u16string>(InputSource::resource("Exodus_25_8_FR.txt"));
      break;
  }

  // ---

  helix = Helix(font);
  helix.update(R1, R2, TURNS, H, DD1, DD2);

  font->setAxis(mirror ? + 1 : -1, +1);

  font->beginSequence(sequence);
  helix.drawText(line, R1, R2, TURNS, H, 0, fontSize1, fontSize2);
  font->endSequence();

  // ---

  spiral1.setup(surface, 0, 0, 0, 600, 160, 0.125f, 15, GRAY_SEA_1);
  spiral2.setup(surface, 0, 0, 0, 800, 80, 0.125f, 25, GRAY_SEA_2);

  // ---

  view = View(clock, 60 * D2R, 1, 3000, mirror),
  view.resize(windowSize); // XXX
  view.add(glm::quat(0.78745633f, -0.61636084f, 0.0022679255f, 0.0025640586f), 60, glm::vec3(120, 0, 0));
  view.add(glm::quat(0.78745633f, -0.61636084f, 0.0022679255f, 0.0025640586f), 60, glm::vec3(60, 0, 0));
  view.add(glm::quat(cosf(30 * D2R), -0.5f, 0, 0), 143, glm::vec3(0, 0, 0));
  view.add(glm::quat(cosf(30 * D2R), -0.5f, 0, 0), 375, glm::vec3(0, 0, 0));

  fogManager.add(COLOR_BACK, 0.15f);
  fogManager.add(COLOR_BACK, 0);

  fadeManager.resize(windowSize); // XXX
}

void Episode1A::enter()
{
  step = 0;

  view.interpolate(0, 1, 16);
  fogManager.interpolate(0, 1, 4);
  fadeManager.reset();

  // ---

  font->setShader(textureAlphaShader);

  // ---

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
}

void Episode1A::update()
{
  switch (step)
  {
    case 0 :
      if (clock->getTime() >= 15.75)
      {
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_OUT, 0.25);
        step = 1;
        break;
      }

    case 1 :
      if (clock->getTime() >= 16)
      {
        view.interpolate(2, 3, 16, true);
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_IN, 0.25);
        step = 2;
      }
      break;

    case 2 :
      if (clock->getTime() >= 31)
      {
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_OUT, 1);
        step = 3;
      }
      break;

    case 3 :
      if (clock->getTime() >= 32)
      {
        nextEpisode();
      }
      break;
  }

  if (step < 2)
  {
    azimuth1 = 45 * D2R;
    spiral1.update(surface, clock->getTime(), 1);
  }
  else
  {
    azimuth2 = (45 + clock->getTime() * 30) * D2R;
    spiral2.update(surface, clock->getTime(), 1);
  }
}

void Episode1A::draw()
{
  glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // ---

  glm::mat4x4 projectionMatrix;
  Matrix modelViewMatrix;
  tie(projectionMatrix, modelViewMatrix) = view.run();

  if (step < 2)
  {
    modelViewMatrix.rotateZ(azimuth1);
  }
  else
  {
    modelViewMatrix.rotateZ(azimuth2);
  }

  State state;
  state
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix);

  fogManager.run(state);
  state.apply();

  if (step < 2)
  {
    spiral1.draw(fogColorShader);
  }
  else
  {
    spiral2.draw(fogColorShader);
    modelViewMatrix.translate(spiral2.getVertex(0));

    state
      .setShaderMatrix(modelViewMatrix * projectionMatrix)
      .apply();

    helix.wireBatch
      .setPrimitive(GL_LINE_STRIP)
      .setShaderColor(COLOR_COLUMN_WIRE)
      .flush();

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);

    glFrontFace(mirror ? GL_CCW : GL_CW);
    font->setColor(COLOR_COLUMN_TEXT_DARK);
    font->replaySequence(sequence);

    glFrontFace(mirror ? GL_CW : GL_CCW);
    font->setColor(COLOR_COLUMN_TEXT_LIGHT);
    font->replaySequence(sequence);
  }

  //

  fadeManager.run();
}
