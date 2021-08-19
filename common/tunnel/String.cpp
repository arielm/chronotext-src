#include "common/tunnel/String.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace path;

String::String(int capacity)
:
size(0),
obstacleBegin(nullptr),
obstacleEnd(nullptr),
locked(false),
lockId(-1),
pushed(false),
pushingObstacleBegin(nullptr),
pushingObstacleEnd(nullptr),
data(nullptr)
{
  w = new float[capacity];
  position = new float[capacity];
  prevPosition = new float[capacity];
  a = new float[capacity];
  dx = new float[capacity];
  dy = new float[capacity];
  d = new float[capacity];
}

String::~String()
{
  LOGD << "String DELETED" << endl;

  delete[] w;
  delete[] position;
  delete[] prevPosition;
  delete[] a;
  delete[] dx;
  delete[] dy;
  delete[] d;

  if (data)
  {
    delete data;
  }
}

void String::setPosition(int index, float _position, float _w, bool fixed)
{
  position[index] = _position;
  w[index] = _w;

  if (fixed)
  {
    prevPosition[index] = _position;
  }

  if (index == 0)
  {
    d[0] = 0;
  }
  else
  {
    d[index] = _position - position[index - 1];
  }
}

void String::setPositions(float _position)
{
  for (int i = 0; i < size; i++)
  {
    _position += d[i]; // d[0] = 0
    position[i] = prevPosition[i] = _position;
  }
}

void String::updateValues(const FollowablePath2D &path)
{
  for (int i = 0; i < size; i++)
  {
    const glm::vec2 gradient = path.offsetToTangent(position[i], w[i]);
    dx[i] = gradient.x;
    dy[i] = gradient.y;
  }
}

void String::accumulateForces(float ax, float ay, float gravity)
{
  for (int i = 0; i < size; i++)
  {
    float _dx = dx[i];
    float _dy = dy[i];
    float _d = sqrtf(_dx * _dx + _dy * _dy);

    a[i] = gravity * (ax * _dx + ay * _dy) / _d;
  }
}

void String::verlet(float dt, float friction)
{
  for (int i = 0; i < size; i++)
  {
    float tmp = position[i];
    position[i] = (2 - friction) * position[i] - (1 - friction) * prevPosition[i] + a[i] * dt;
    prevPosition[i] = tmp;
  }
}
