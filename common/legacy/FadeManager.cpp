#include "FadeManager.h"

#include "chr/gl/Batch.h"
#include "chr/gl/draw/Rect.h"

using namespace chr;
using namespace gl;

FadeManager::FadeManager(Clock::Ref masterClock)
:
clock(masterClock)
{}

void FadeManager::resize(const glm::vec2 &size)
{
  windowSize = size;
}

void FadeManager::reset()
{
  interpolating = false;
}

void FadeManager::interpolate(const glm::vec3 &color, Mode mode, double duration)
{
  FadeManager::color = color;
  FadeManager::mode = mode;
  FadeManager::duration = duration;

  interpolating = true;
  t0 = clock->getTime();
}

void FadeManager::run()
{
  if (interpolating)
  {
    float u = fminf((clock->getTime() - t0) / duration, 1);

    if (mode == FADE_IN)
    {
      u = 1 - u;
    }

    //

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(false);

    //

    State()
      .setShaderMatrix(glm::ortho(0.0f, windowSize.x, windowSize.y, 0.0f))
      .apply();

    //

    IndexedVertexBatch<XYZ.RGBA> colorBatch;
    colorBatch.setShader(colorShader);

    draw::Rect()
      .setColor(color.r, color.g, color.b, u)
      .setBounds(0, 0, windowSize.x, windowSize.y)
      .append(colorBatch, Matrix());

    colorBatch.flush();
  }
}
