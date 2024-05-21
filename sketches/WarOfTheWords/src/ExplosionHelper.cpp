#include "ExplosionHelper.h"

using namespace chr;
using namespace gl;

static constexpr float RADIUS_MAX = 60; // RELATIVE TO Missile::explosionRadius DEFINED IN MissileManager::createMissile()
static constexpr int SEGMENT_COUNT = 30;

void ExplosionHelper::draw(const glm::vec3 &center, float factor, float radius)
{
  explosions.emplace_back(center, factor, radius);
}

void ExplosionHelper::flush(DrawingContext context)
{
  if (context.explosionBatch.empty())
  {
    drawHalfSphere(context.explosionBatch, RADIUS_MAX * 0.25f, SEGMENT_COUNT);
    drawHalfSphere(context.explosionBatch, RADIUS_MAX * 0.5f, SEGMENT_COUNT);
    drawHalfSphere(context.explosionBatch, RADIUS_MAX * 0.75f, SEGMENT_COUNT);
    drawHalfSphere(context.explosionBatch, RADIUS_MAX, SEGMENT_COUNT);
  }

  InstanceBuffer instanceBuffer(GL_STATIC_DRAW, GL_STATIC_DRAW);
  for (const auto &explosion : explosions)
  {
    instanceBuffer.addColor(1, 0.5f, 0, 0.9f * math::easeOut(1 - explosion.factor));

    instanceBuffer.addMatrix(Matrix()
     .setTranslate(explosion.center)
     .scale(explosion.factor * explosion.radius / RADIUS_MAX));
  }

  context.explosionBatch.flush(instanceBuffer);

  explosions.clear();
}

void ExplosionHelper::drawHalfSphere(IndexedVertexBatch<> &batch, float radius, int segmentCount)
{
  int sliceCount = (int)ceilf(segmentCount * 0.25f) + 1;

  for (int iz = 0; iz < sliceCount; iz++)
  {
    float theta1 = iz * TWO_PI / segmentCount - HALF_PI;
    float cosTheta1 = cosf(theta1);
    float sinTheta1 = sinf(theta1);

    for (int ix = 0; ix < segmentCount; ix++)
    {
      float theta2 = ix * TWO_PI / segmentCount;
      batch.addVertex(glm::vec3(cosTheta1 * cosf(theta2), cosTheta1 * sinf(theta2), -sinTheta1) * radius);
    }
  }

  for (int iy = 0; iy < sliceCount - 1; iy++)
  {
    int i = iy * segmentCount;

    for (int ix = 0; ix < segmentCount; ix++)
    {
      batch
        .addIndices(i + segmentCount + ix, i + ix, i + (ix + 1) % segmentCount)
        .addIndices(i + (ix + 1) % segmentCount, i + segmentCount + (ix + 1) % segmentCount, i + segmentCount + ix);
    }
  }

  batch.incrementIndices(segmentCount * sliceCount);
}
