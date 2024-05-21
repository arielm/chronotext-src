#include "MissileManager.h"

using namespace std;
using namespace chr;

void MissileManager::reset()
{
  missiles.clear();
  numActiveMissiles = 0;
}

shared_ptr<Missile> MissileManager::createMissile(Random &random, float elapsed, float x, float y, float z, const Target &target)
{
  auto missile = make_shared<Missile>(x, y, z);
  missiles.push_back(missile);

  missile->target = target;

  missile->t0 = elapsed;
  missile->dt = DT;
  missile->friction = FRICTION;

  missile->r0 = random.nextFloat() * TWO_PI;
  missile->dr = (30 + 30 * random.nextFloat()) * D2R;

  missile->propulsionDirection = { -0.3f + 0.6f * random.nextFloat(), -0.3f + 0.6f * random.nextFloat(), 10 + 5 * random.nextFloat() };
  missile->propulsionDuration = 2.3f + random.nextFloat(0.4f);

  missile->approachAngle = (7 + random.nextFloat() * 7) * D2R;
  missile->approachDistance = 1000;
  missile->approachVelocity = 9 + random.nextFloat() * 2;

  missile->freeFlightDuration = 1 + random.nextFloat(1);

  missile->explosionDuration = 1 + random.nextFloat(0.5f);
  missile->explosionRadius = 40 + 20 * random.nextFloat();

  return missile;
}

int MissileManager::getNumActiveMissiles()
{
  return numActiveMissiles;
}

void MissileManager::run(float elapsed)
{
  numActiveMissiles = 0;

  for (auto it = missiles.begin(); it != missiles.end();)
  {
    auto missile = it->get();

    if (!missile->out)
    {
      numActiveMissiles++;
      missile->run(elapsed);
      ++it;
    }
    else
    {
      it = missiles.erase(it);
    }
  }
}

void MissileManager::draw(DrawingContext &context)
{
  for (auto missile : missiles)
  {
    if (missile->launched)
    {
      missile->draw(context, explosionHelper);
    }
  }
}
