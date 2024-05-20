#include "View.h"

using namespace std;
using namespace chr;
using namespace gl;

View::View(Clock::Ref masterClock, float fovy, float znear, float zfar, bool mirror)
:
clock(masterClock),
fovy(fovy),
znear(znear),
zfar(zfar),
mirror(mirror),
currentValue(glm::quat(1, 0, 0, 0), 100, glm::vec3(0))
{}

void View::resize(const glm::vec2 &size)
{
  aspectRatio = size.x / size.y;
}

void View::add(const glm::quat &quat, float distance, const glm::vec3 &center)
{
  values.emplace_back(quat, distance, center);
}

void View::set(int index)
{
  currentValue = values[index];
  interpolating = false;
}

void View::interpolate(int fromIndex, int toIndex, double duration, bool easing)
{
  View::fromIndex = fromIndex;
  View::toIndex = toIndex;
  View::duration = duration;
  View::easing = easing;

  set(fromIndex);

  interpolating = true;
  t0 = clock->getTime();
}

pair<glm::mat4x4, Matrix> View::run()
{
  if (interpolating)
  {
    float u = fminf((clock->getTime() - t0) / duration, 1);

    if (easing)
    {
      u = math::ease(u);
    }

    currentValue.quat = glm::slerp(values[fromIndex].quat, values[toIndex].quat, u);
    currentValue.distance = math::lerp(values[fromIndex].distance, values[toIndex].distance, u);
    currentValue.center = glm::lerp(values[fromIndex].center, values[toIndex].center, u);
  }

  auto projectionMatrix = glm::perspective(fovy, aspectRatio, znear, zfar);

  Matrix modelViewMatrix;
  modelViewMatrix
    .scale(mirror ? -1 : +1, 1)
    .translate(0, 0, -currentValue.distance)
    .applyQuat(currentValue.quat)
    .translate(currentValue.center);

  return make_pair(projectionMatrix, modelViewMatrix);
}
