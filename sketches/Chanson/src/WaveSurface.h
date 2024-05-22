#pragma once

class WaveSurface
{
public:
  WaveSurface(float amplitude, float period, float velocity)
  :
  amplitude(amplitude),
  period(period),
  velocity(velocity)
  {}

  float getHeight(float x, float y, float t);

protected:
  float amplitude;
  float period; // DISTANCE BETWEEN CRESTS IN PIXELS
  float velocity;; // TIME BETWEEN CRESTS IN PIXELS/SECOND
};
