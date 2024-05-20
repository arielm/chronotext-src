#pragma once

#include "chr/path/FollowablePath2D.h"
#include "chr/path/SplinePath.h"

class CompositePath
{
public:
  CompositePath();

  void update(chr::path::SplinePath<glm::vec2> &spline);
  float getOffset(float x) const;

  const std::vector<chr::path::FollowablePath2D::Point>& getPoints() const;
  float getLength() const;
  chr::path::FollowablePath2D::Value offsetToValue(float offset, float sampleSize = 0) const;

protected:
  chr::path::FollowablePath2D followablePath;
  std::vector<float> positionX;
  std::vector<float> theta;
};
