#pragma once

#include "DrawingContext.h"

class ExplosionHelper
{
public:
  void draw(const glm::vec3 &center, float factor, float radius);
  void flush(DrawingContext context);

protected:
  struct Explosion
  {
    glm::vec3 center;
    float factor;
    float radius;

    Explosion(glm::vec3 center, float factor, float radius)
    :
    center(center),
    factor(factor),
    radius(radius)
    {}
  };

  std::vector<Explosion> explosions;

  void drawHalfSphere(chr::gl::IndexedVertexBatch<> &batch, float radius, int segmentCount);
};
