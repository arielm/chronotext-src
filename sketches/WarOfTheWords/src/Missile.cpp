#include "Missile.h"

#include "TextSpan.h"

using namespace std;
using namespace chr;
using namespace gl;

Missile::Missile(float x, float y, float z)
:
x(x),
y(y),
z(z),
x0(x),
y0(y),
z0(z),
originX(x),
originY(y),
originZ(z)
{
  for (int i = 0; i < NN; i++)
  {
    nx[i] = x;
    ny[i] = y;
    nz[i] = z - i * DD;
  }
}

void Missile::launch()
{
  launched = true;
}

void Missile::approach()
{
  float dx1 = target.x - originX;
  float dy1 = target.y - originY;
  float d1 = (sqrtf(dx1 * dx1 + dy1 * dy1));

  float b = d1 / 2;
  float a = fmaxf(approachHeightMin, b / atan(approachAngle));

  float midX = originX + dx1 / 2;
  float midY = originY + dy1 / 2;
  float midZ = a;

  float dx = target.x - midX;
  float dy = target.y - midY;
  float dz = target.z - midZ;
  float d = sqrtf(dx * dx + dy * dy + dz * dz);
  dx /= d;
  dy /= d;
  dz /= d;

  x = target.x - approachDistance * dx;
  y = target.y - approachDistance * dy;
  z = target.z - approachDistance * dz;

  x0 = x - dx * approachVelocity;
  y0 = y - dy * approachVelocity;
  z0 = z - dz * approachVelocity;

  for (int i = 0; i < NN; i++)
  {
    nx[i] = x - i * dx * DD;
    ny[i] = y - i * dy * DD;
    nz[i] = z - i * dz * DD;
  }

  friction = 0;
  approaching = true;
}

void Missile::accumulateForces(float t)
{
  ax = 0;
  ay = 0;
  az = 0;

  if (!approaching)
  {
    if ((t - t0) <= propulsionDuration)
    {
      ax += propulsionDirection.x;
      ay += propulsionDirection.y;
      az += propulsionDirection.z;
    }
  }
}

void Missile::verlet(float dt)
{
  float tmp = x;
  x = (2 - friction) * x - (1 - friction) * x0 + ax * dt * dt;
  x0 = tmp;

  tmp = y;
  y = (2 - friction) * y - (1 - friction) * y0 + ay * dt * dt;
  y0 = tmp;

  tmp = z;
  z = (2 - friction) * z - (1 - friction) * z0 + az * dt * dt;
  z0 = tmp;
}

void Missile::satisfyConstraints(float t)
{}

void Missile::run(float elapsed)
{
  accumulateForces(elapsed);
  verlet(dt);
  satisfyConstraints(elapsed);

  float t = elapsed - t0;

  if (exploding)
  {
    if (t <= explosionDuration)
    {
      explosionFactor = fminf(1, 0.125f + t / explosionDuration); // XXX
    }
    else
    {
      out = true;
    }
  }
  else
  {
    if (approaching)
    {
      if (z <= target.z)
      {
        t0 = elapsed;
        exploding = true;
        explosionCenter =  { x, y, target.z };

        if (target.span)
        {
          target.span->collapse(elapsed);
        }
      }
    }
    else if (t > propulsionDuration + freeFlightDuration)
    {
      approach();
    }
  }
}

void Missile::draw(DrawingContext &context, ExplosionHelper &explosionHelper)
{
  if (exploding)
  {
    explosionHelper.draw(explosionCenter, explosionFactor, explosionRadius);
  }

  if (!approaching || nz[NN - 1] > target.z)
  {
    nx[0] = x;
    ny[0] = y;
    nz[0] = z;

    for (int i = 1; i < NN; i++)
    {
      float dx = nx[i - 1] - nx[i];
      float dy = ny[i - 1] - ny[i];
      float dz = nz[i - 1] - nz[i];
      float len = sqrtf(dx * dx + dy * dy + dz * dz);

      nx[i] = nx[i - 1] - dx / len * DD;
      ny[i] = ny[i - 1] - dy / len * DD;
      nz[i] = nz[i - 1] - dz / len * DD;
    }

    // ---

    glm::vec4 color0, color1;

    if (approaching)
    {
      color0 = { 0, 0, 0, 0.5f };
      color1 = { 0, 0, 0, 0 };
    }
    else
    {
      color0 = { 0.5f, 0, 0, 0.5f };
      color1 = { 0.75f, 0.5f, 0, 0 };
    }

    Matrix matrix;
    matrix.translate(nx[0], ny[0], nz[0]);

    for (int i = 0; i < NN - 1; i++)
    {
      float u = i / (float) NN;

      glm::vec4 color = (1 - u) * color0 + u * color1;

      matrix.rotateZ(r0 + u * dr);

      Vertex<XYZ.RGBA> v0(matrix.transformPoint(0, -0.5f * WW * u, 0), color);
      Vertex<XYZ.RGBA> v1(matrix.transformPoint(0, +0.5f * WW * u, 0), color);

      if ((i == 0) && (!context.missileBatch.empty()))
      {
        context.missileBatch.addVertex(v0); // ADDING FIRST VERTEX TO CREATE DEGENERATE STRIP (PART 2 OF 2)
      }

      context.missileBatch
        .addVertex(v0)
        .addVertex(v1);

      if (i == NN - 2)
      {
        context.missileBatch.addVertex(v1); // ADDING LAST VERTEX TO CREATE DEGENERATE STRIP (PART 1 OF 2)
      }

      matrix.clearRotation();

      float dx = nx[i + 1] - nx[i];
      float dy = ny[i + 1] - ny[i];
      float dz = nz[i + 1] - nz[i];
      float ry = +atan2f(dx, dz);
      float rx = -atan2f(dy, DD);

      matrix
        .rotateY(ry)
        .rotateX(rx)
        .translate(0, 0, DD)
        .clearRotation();
    }
  }
}
