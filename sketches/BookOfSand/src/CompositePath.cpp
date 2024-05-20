#include "CompositePath.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace path;

CompositePath::CompositePath()
{
  followablePath.setMode(FollowablePath2D::MODE_TANGENT);
}

void CompositePath::update(SplinePath<glm::vec2> &spline)
{
  followablePath
    .begin()
    .add(spline.getPolyline())
    .end();

  // ---

  positionX.clear();
  theta.clear();

  for (const auto &point : followablePath.getPoints())
  {
    positionX.push_back(point.position.x);
    theta.push_back(atan2f(point.forward.y, point.forward.x)); // XXX: We only need the 1st and the last angles
  }
}

float CompositePath::getOffset(float x) const
{
  int size = positionX.size();
  const auto &lengths = followablePath.getLengths();

  if (x < positionX[0])
  {
    float d = x - positionX[0];
    return lengths[0] + d / cosf(theta[0]);
  }
  else if (x >= positionX[size - 1])
  {
    float d = x - positionX[size - 1];
    return lengths[size - 1] + d / cosf(theta[size - 1]);
  }

  int index = utils::search(positionX, x, 1, size);
  float ratio = (x - positionX[index]) / (positionX[index + 1] - positionX[index]);
  return lengths[index] + (lengths[index + 1] - lengths[index]) * ratio;
}

const vector<FollowablePath2D::Point>& CompositePath::getPoints() const
{
  return followablePath.getPoints();
}

float CompositePath::getLength() const
{
  return followablePath.getLength();
}

FollowablePath2D::Value CompositePath::offsetToValue(float offset, float sampleSize) const
{
  return followablePath.offsetToValue(offset, sampleSize);
}
