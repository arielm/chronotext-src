#pragma once

#include "Missile.h"
#include "DrawingContext.h"
#include "ExplosionHelper.h"

#include "chr/Random.h"

#include <list>

static constexpr float DT = 0.1f;
static constexpr float FRICTION = 0.0125f;

class MissileManager
{
public:
  std::list<std::shared_ptr<Missile>> missiles;
  ExplosionHelper explosionHelper;

  void reset();

  std::shared_ptr<Missile> createMissile(chr::Random &random, float elapsed, float x, float y, float z, const Target &target);
  int getNumActiveMissiles();

  void run(float elapsed);
  void draw(DrawingContext &context);

protected:
  int numActiveMissiles = 0;
};
