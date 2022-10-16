#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;

static constexpr int64_t NOISE_SEED = 12345678;
static constexpr int NOISE_OCTAVES = 3;
static constexpr float NOISE_SCALE = 0.04f;
static constexpr float TERRAIN_H = 25;

static constexpr float R1 = 0;
static constexpr float R2 = 120;
static constexpr float TURNS = 50;
static constexpr float DD1 = 2.5f;
static constexpr float DD2 = 2.5f;

Sketch::Sketch()
:
surface(400, 4, NOISE_SCALE, NOISE_OCTAVES, NOISE_SEED),
lineBatch(GL_LINE_STRIP, spiral.vertexBuffer)
{}

void Sketch::setup()
{
  auto lines = utils::readLines<u16string>(InputSource::resource("Isaiah_40.txt"));
  for (const auto &line : lines)
  {
    text += line + u" ";
  }

  font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d());
  font->setDirection(-1);
  font->setSize(3);
  font->setShader(textureAlphaShader);
  font->setColor(0, 0, 0, 1);

  // ---

  spiral.enableWire(true);
  spiral.enablePath(true);
  spiral.setSampleSize(1.5f);
  spiral.setSamplingTolerance(0.0025f);
  spiral.path.setSampling(FollowablePath3D::SAMPLING_CONTINUOUS);
  spiral.setDirection(font->getDirection());
  spiral.setup(R1, R2, TURNS, DD1, DD2);

  surface.generateBase();
  generateSpiral();

  animClock->start();

  // ---

  lineBatch
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 0.4f);

  // ---

  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::draw()
{
  glClearColor(1.0f, 0.9f, 0.7f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // ---

  auto now = animClock->getTime();
  float azimuth = now * 2;
  float distance = 90 - now * 3;

  if (distance < 30)
  {
    animClock->restart();
  }

  auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.aspectRatio(), 1.0f, 1000.0f);

  Matrix viewMatrix;
  viewMatrix
    .translate(0, 0, -distance)
    .rotateX(-60 * D2R)
    .rotateZ(azimuth * D2R);

  State()
    .setShaderMatrix<MVP>(viewMatrix * projectionMatrix)
    .apply();

  lineBatch.flush();
  font->replaySequence(sequence);
}

void Sketch::mouseDragged(int button, float x, float y)
{
  mousePressed(button, x, y);
}

void Sketch::mousePressed(int button, float x, float y)
{
  reseed(x * y);
}

void Sketch::generateSpiral()
{
  spiral.update(surface, TERRAIN_H);

  float offset = 0;
  float offsetY = font->getOffsetY(XFont::ALIGN_MIDDLE);
  Matrix matrix;

  font->beginSequence(sequence);

  for (auto c : text)
  {
    int glyphIndex = font->getGlyphIndex(c);
    float halfWidth = font->getGlyphAdvance(glyphIndex) / 2;
    offset += halfWidth;

    if (glyphIndex >= 0)
    {
      spiral.path
        .offsetToValue(offset, halfWidth * 2)
        .applyToMatrix(matrix);

      if (font->getDirection() < 0)
      {
        matrix.rotateZ(PI);
      }

      font->addGlyph(matrix, glyphIndex, -halfWidth * font->getDirection(), offsetY);
    }

    offset += halfWidth;
  }

  font->endSequence();
}

void Sketch::reseed(int64_t seed)
{
  surface.reseed(seed);
  generateSpiral();
}
