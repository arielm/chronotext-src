#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr int64_t NOISE_SEED = 12345678;
static constexpr int NOISE_OCTAVES = 3;
static constexpr float NOISE_SCALE = 0.04f;
static constexpr float TERRAIN_H = 25;

static constexpr float R1 = 0;
static constexpr float R2 = 120;
static constexpr float TURNS = 50;
static constexpr float DD1 = 1.0f;
static constexpr float DD2 = 3.0f;

Sketch::Sketch()
:
noise(NOISE_SEED, NOISE_OCTAVES)
{}

void Sketch::setup()
{
  lineBatch
    .setPrimitive(GL_LINE_STRIP)
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 0.4f);

  updateSpiral();

  animClock->start();

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
  float distance = 250 - now * 3;

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
}

void Sketch::mouseDragged(int button, float x, float y)
{
  mousePressed(button, x, y);
}

void Sketch::mousePressed(int button, float x, float y)
{
  reseed(x * y);
}

void Sketch::reseed(int64_t seed)
{
  noise.reseed(seed);
  updateSpiral();
}

void Sketch::updateSpiral()
{
  lineBatch.clear();

  float l = TWO_PI * TURNS;
  float L = PI * TURNS * (R1 + R2);
  float dr = (R2 - R1) / l;
  float DD = (DD2 - DD1) / l;
  float D = 0;

  do
  {
    float r = sqrtf(R1 * R1 + 2 * dr * D);
    float d = (r - R1) / dr;
    D += DD1 + d * DD;

    float x = -sinf(d) * r;
    float y = +cosf(d) * r;

    lineBatch.addVertex(x, y, TERRAIN_H * noise.get(NOISE_SCALE * x, NOISE_SCALE * y));
  }
  while (D < L);
}
