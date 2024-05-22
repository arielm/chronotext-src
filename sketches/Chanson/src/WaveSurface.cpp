#include "WaveSurface.h"

#include "chr/math/Utils.h"

using namespace chr;

float WaveSurface::getHeight(float x, float y, float t)
{
  float phase = t * velocity / PI;
  return amplitude * sinf((x - y) * TWO_PI / period - phase);
}
