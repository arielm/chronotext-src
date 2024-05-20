#include "common/ArcBall.h"

ArcBall::ArcBall(const glm::vec2 &center, float radius, float distance, const glm::quat &rotation)
:
center(center),
radius(radius),
distance(distance),
origin(0),
q_now(rotation),
q_down(rotation)
{}

void ArcBall::mousePressed(int buttonIndex, float x, float y)
{
  button_last = buttonIndex;

  if (button_last == 0)
  {
    v_down = mouseToSphere(x, y);
    q_down = q_now;
    q_drag = glm::quat();
  }
  else
  {
    distance_last = distance;
    mouse_y_last = y;
  }
}

void ArcBall::mouseDragged(int buttonIndex, float x, float y)
{
  if (button_last == 0)
  {
    v_drag = mouseToSphere(x, y);
    q_drag = glm::quat(glm::dot(v_down, v_drag), glm::cross(v_down, v_drag));

    q_now = q_drag * q_down;
  }
  else
  {
    distance = distance_last + (mouse_y_last - y) * zoom_factor;
  }
}

float ArcBall::getDistance() const
{
  return distance;
}

glm::quat ArcBall::getRotation() const
{
  return q_now;
}

glm::mat4 ArcBall::getMatrix() const
{
  glm::mat4 m;
  m *= glm::translate(m, origin / distance);
  m *= glm::translate(m, glm::vec3(0, 0, -distance));
  m *= glm::mat4_cast(q_now);
  return m;
}

void ArcBall::setOrigin(const glm::vec3 &origin)
{
  this->origin = origin;
}

glm::vec3 ArcBall::mouseToSphere(float x, float y) const
{
  glm::vec3 v;
  v.x = (x - center.x) / radius;
  v.y = (center.y - y) / radius;

  float mag = glm::length2(v);

  if (mag > 1)
  {
    glm::normalize(v);
  }
  else
  {
    v.z = sqrtf(1 - mag);
  }

  return v;
}
