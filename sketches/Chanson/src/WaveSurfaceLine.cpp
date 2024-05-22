#include "WaveSurfaceLine.h"

using namespace std;
using namespace chr;

WaveSurfaceLine::WaveSurfaceLine(WaveSurface *wave, float y, float width, float segmentLength, float sampleSize)
:
wave(wave),
y(y),
width(width),
segmentLength(segmentLength),
sampleSize(sampleSize),
size(max<int>(1, width / segmentLength)),
path(size)
{}

void WaveSurfaceLine::compute(float t)
{
  float ym = y - sampleSize * 0.5f;
  float yp = y + sampleSize * 0.5f;

  float oy = wave->getHeight(0, y, t);
  float oym = wave->getHeight(0, ym, t);
  float oyp = wave->getHeight(0, yp, t);

  path.begin();

  for (float x = 0; x <= width; x += segmentLength)
  {
    float z = wave->getHeight(x, y, t) - oy;

    float zm = wave->getHeight(x, ym, t) - oym;
    float zp = wave->getHeight(x, yp, t) - oyp;
    glm::vec3 left = glm::normalize(glm::vec3(0, yp - ym, zp - zm));

    path.add(glm::vec3(x, y, z), left);
  }

  path.end();
}
