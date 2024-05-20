#pragma once

#include "chr/time/Clock.h"
#include "chr/gl/State.h"

class FogManager
{
public:
  enum Mode
  {
    MODE_NONE,
    MODE_EXP,
    MODE_EXP2
  };

  FogManager(chr::Clock::Ref masterClock, Mode mode);

  void add(const glm::vec3 &color, float density);
  void set(int index);

  void interpolate(int fromIndex, int toIndex, double duration);
  void run(chr::gl::State &state);

protected:
  struct Value
  {
    glm::vec3 color;
    float density;

    Value(const glm::vec3 &color, float density)
    :
    color(color),
    density(density)
    {}
  };

  Mode mode;

  std::vector<Value> values;
  Value currentValue;

  int fromIndex;
  int toIndex;

  double duration;
  double t0;
  chr::Clock::Ref clock;
  bool interpolating = false;
};
