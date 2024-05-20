#include "common/NoiseSurfaceSpiral1.h"

#include "chr/math/Utils.h"

using namespace std;
using namespace chr;

void NoiseSurfaceSpiral1::setup(float r1, float r2, float turns, float DD1, float DD2)
{
  points.clear();
  pointsM.clear();
  pointsP.clear();

  float l = TWO_PI * turns;
  float L = PI * turns * (r1 + r2);
  float dr = (r2 - r1) / l;
  float DD = (DD2 - DD1) / l;
  float D = 0;

  do
  {
    float r = sqrtf(r1 * r1 + 2 * dr * D);
    float d = (r - r1) / dr;
    D += DD1 + d * DD;

    float sx = -sinf(d * direction);
    float cy = +cosf(d * direction);

    points.emplace_back(sx * r, cy * r);

    if (pathEnabled)
    {
      float rm = r - sampleSize * 0.5f;
      pointsM.emplace_back(sx * rm, cy * rm);

      float rp = r + sampleSize * 0.5f;
      pointsP.emplace_back(sx * rp, cy * rp);
    }
  }
  while (D < L);

  // ---

  if (wireEnabled)
  {
    vertexBuffer.clear();
    vertexBuffer.extendCapacity(points.size());
  }

  if (pathEnabled)
  {
    path.clear();
    path.reserve(points.size());
  }
}

void NoiseSurfaceSpiral1::update(const NoiseSurface &surface, float height, const glm::vec2 &offset)
{
  auto &vertices = vertexBuffer->storage;

  if (wireEnabled)
  {
    vertexBuffer.clear();
  }

  if (pathEnabled)
  {
    path.begin();
  }

  for (size_t i = 0, size = points.size(); i < size; i++)
  {
    const glm::vec3 p(points[i], height * surface.getHeight(points[i] + offset));

    if (wireEnabled)
    {
      vertices.emplace_back(p);
    }

    if (pathEnabled)
    {
      const glm::vec3 pm(pointsM[i], height * surface.getHeight(pointsM[i] + offset));
      const glm::vec3 pp(pointsP[i], height * surface.getHeight(pointsP[i] + offset));

      path.add(p, glm::normalize(pp - pm));
    }
  }

  if (pathEnabled)
  {
    path.end();
  }
}

void NoiseSurfaceSpiral1::enableWire(bool enable)
{
  wireEnabled = enable;
}

void NoiseSurfaceSpiral1::enablePath(bool enable)
{
  pathEnabled = enable;
}

void NoiseSurfaceSpiral1::setDirection(float direction)
{
  this->direction = direction;
}

void NoiseSurfaceSpiral1::setSampleSize(float size)
{
  sampleSize = size;
}
