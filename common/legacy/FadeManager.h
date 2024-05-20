#pragma once

#include "chr/time/Clock.h"
#include "chr/gl/shaders/ColorShader.h"

class FadeManager
{
public:
  enum Mode
  {
    FADE_IN = 1,
    FADE_OUT = 2
  };

  FadeManager(chr::Clock::Ref masterClock);

  void resize(const glm::vec2 &size);
  void reset();

  void interpolate(const glm::vec3 &color, Mode mode, double duration);
  void run();

protected:
  chr::gl::shaders::ColorShader colorShader;
  glm::vec2 windowSize;

  glm::vec3 color;
  int mode;

  double duration;
  double t0;
  chr::Clock::Ref clock;
  bool interpolating = false;
};
