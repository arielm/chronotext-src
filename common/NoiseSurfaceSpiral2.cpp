#include "common/NoiseSurfaceSpiral2.h"

#include "chr/math/Utils.h"

using namespace std;
using namespace chr;
using namespace path;

NoiseSurfaceSpiral2::NoiseSurfaceSpiral2()
{
  spline.setType(SplinePath<glm::vec3>::TYPE_BSPLINE);
}

void NoiseSurfaceSpiral2::setup(float r1, float r2, float turns, float DD1, float DD2)
{
  points.clear();

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

    points.emplace_back(-sinf(d * direction) * r, +cosf(d * direction) * r);
  }
  while (D < L);
}

void NoiseSurfaceSpiral2::update(const NoiseSurface &surface, float height, const glm::vec2 &offset)
{
  spline
    .setSamplingTolerance(samplingTolerance)
    .clear();

  for (const auto &point : points)
  {
    glm::vec3 p(point, height * surface.getHeight(point + offset));
    spline.add(p);
  }

  const auto &polyline = spline.getPolyline();

  if (wireEnabled)
  {
    vertexBuffer.clear();

    auto &vertices = vertexBuffer->storage;
    vertices.reserve(polyline.size());

    for (const auto &point : polyline)
    {
      vertices.push_back(point);
    }
  }

  if (pathEnabled)
  {
    glm::vec2 lastTangent;

    path.clear();
    path.reserve(polyline.size());

    path.begin();

    for (int i = 0, size = polyline.size(); i < size; i++)
    {
      const auto &p = polyline[i];

      glm::vec2 tangent;
      glm::vec2 p0(p);

      if (i == size - 1)
      {
        tangent = lastTangent;
      }
      else
      {
        glm::vec2 p1(polyline[i + 1]);
        tangent = glm::normalize(p1 - p0);
      }

      glm::vec2 ortho(tangent.yx() * glm::vec2(-1, +1));
      glm::vec2 pm0(p0 + ortho * sampleSize * 0.5f);
      glm::vec2 pp0(p0 - ortho * sampleSize * 0.5f);

      glm::vec3 pm(pm0, height * surface.getHeight(pm0 + offset));
      glm::vec3 pp(pp0, height * surface.getHeight(pp0 + offset));

      path.add(p, glm::normalize(pp - pm));
      lastTangent = tangent;
    }

    path.end();
  }
}

void NoiseSurfaceSpiral2::enableWire(bool enable)
{
  wireEnabled = enable;
}

void NoiseSurfaceSpiral2::enablePath(bool enable)
{
  pathEnabled = enable;
}

void NoiseSurfaceSpiral2::setDirection(float direction)
{
  this->direction = direction;
}

void NoiseSurfaceSpiral2::setSampleSize(float size)
{
  sampleSize = size;
}

void NoiseSurfaceSpiral2::setSamplingTolerance(float tolerance)
{
  samplingTolerance = tolerance;
}
