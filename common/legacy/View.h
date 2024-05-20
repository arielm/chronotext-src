#pragma once

#include "chr/time/Clock.h"
#include "chr/gl/Matrix.h"

class View
{
public:
  View() = default;
  View(chr::Clock::Ref masterClock, float fovy, float znear, float zfar, bool mirror = false);

  void resize(const glm::vec2 &size);

  void add(const glm::quat &quat, float distance, const glm::vec3 &center);
  void set(int index);

  void interpolate(int fromIndex, int toIndex, double duration, bool easing = false);
  std::pair<glm::mat4x4, chr::gl::Matrix> run();

protected:
  struct Value
  {
    glm::quat quat;
    float distance;
    glm::vec3 center;

    Value() = default;

    Value(const glm::quat &quat, float distance, const glm::vec3 &center)
    :
    quat(quat),
    distance(distance),
    center(center)
    {}
  };

  float aspectRatio;
  float fovy;
  float znear;
  float zfar;
  bool mirror;

  std::vector<Value> values;
  Value currentValue;

  int fromIndex;
  int toIndex;

  double duration;
  double t0;
  chr::Clock::Ref clock;
  bool interpolating = false;
  bool easing;
};
