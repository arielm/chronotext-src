#include "FogManager.h"

#include "chr/math/Utils.h"

using namespace chr;
using namespace gl;

FogManager::FogManager(Clock::Ref masterClock, Mode mode)
:
clock(masterClock),
mode(mode),
currentValue(glm::vec3(0, 0, 0), 0)
{}

void FogManager::add(const glm::vec3 &color, float density)
{
  values.emplace_back(color, density);
}

void FogManager::set(int index)
{
  currentValue = values[index];
  interpolating = false;
}

void FogManager::interpolate(int fromIndex, int toIndex, double duration)
{
  FogManager::fromIndex = fromIndex;
  FogManager::toIndex = toIndex;
  FogManager::duration = duration;

  set(fromIndex);

  interpolating = true;
  t0 = clock->getTime();
}

void FogManager::run(State &state)
{
  if (interpolating)
  {
    float u = fminf((clock->getTime() - t0) / duration, 1);

    currentValue.color = glm::lerp(values[fromIndex].color, values[toIndex].color, u);
    currentValue.density = math::lerp(values[fromIndex].density, values[toIndex].density, u);
  }

  state
    .setShaderUniform("u_fogSelector", mode)
    .setShaderUniform("u_fogDensity", currentValue.density)
    .setShaderUniform("u_fogColor", currentValue.color);
}
