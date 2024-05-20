#pragma once

#include "chr/glm.h"

class ArcBall
{
public:
  ArcBall() = default;
  ArcBall(const glm::vec2 &center, float radius, float distance, const glm::quat &rotation);

  void mousePressed(int buttonIndex, float x, float y);
  void mouseDragged(int buttonIndex, float x, float y);

  float getDistance() const;
  glm::quat getRotation() const;
  glm::mat4 getMatrix() const;

  void setOrigin(const glm::vec3 &origin); // XXX
  inline void setOrigin(float x, float y, float z) { setOrigin(glm::vec3(x, y, z)); }

protected:
  glm::vec2 center;
  float radius;
  glm::vec3 v_down, v_drag;
  glm::quat q_now, q_down, q_drag;
  glm::vec3 origin;

  float distance;
  float distance_last;
  int mouse_y_last;
  int button_last;
  float zoom_factor = 1;

  glm::vec3 mouseToSphere(float x, float y) const;
};
