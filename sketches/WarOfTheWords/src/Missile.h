#pragma once

#include "Target.h"
#include "DrawingContext.h"
#include "ExplosionHelper.h"

static constexpr int NN = 50; // NUMBER OF NODES
static constexpr float DD = 4; // DISTANCE BETWEEN EACH NODE
static constexpr float WW = 4; // STRIP WIDTH

class Missile
{
public:
  Target target;

  float x, y, z;
  float x0, y0, z0;
  float ax = 0, ay = 0, az = 0;
  float originX, originY, originZ;

  float t0;
  float dt;
  float friction;

  float r0; // ROTATION ANGLE AROUND Z-AXIS, AT BEGINNING OF STRIP
  float dr; // AMOUNT OF ROTATION AROUND Z-AXIS, ALONG THE STRIP

  glm::vec3 propulsionDirection;
  float propulsionDuration; // IN SECONDS

  float freeFlightDuration; // IN SECONDS

  float approachAngle; // IN RADIANS
  float approachDistance, approachVelocity;
  static constexpr float approachHeightMin = 1000; // REQUIRED IN CASES WHERE LAUNCHER AND TARGET ARE VERY CLOSE

  bool launched = false, out = false;
  bool approaching = false, exploding = false;

  float explosionDuration; // IN SECONDS
  float explosionRadius;
  glm::vec3 explosionCenter;
  float explosionFactor = 0;

  float nx[NN];
  float ny[NN];
  float nz[NN];

  std::vector<glm::vec3> points;

  Missile() = default;
  Missile(float x, float y, float z);

  void launch();
  void approach();

  void accumulateForces(float t);
  void verlet(float dt);
  void satisfyConstraints(float t);

  void run(float elapsed);
  void draw(DrawingContext &context, ExplosionHelper &explosionHelper);
};
