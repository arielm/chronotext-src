/*
 * Based on https://github.com/cinder/Cinder/blob/master/include/cinder/Camera.h
 */

#pragma once

#include "chr/gl/Matrix.h"

class Camera
{
public:
  void setEyePoint(const glm::vec3 &eyePoint);
  void setOrientation(const glm::vec3 &axis, float radians);
  void setPerspective(float fovy, float aspectRatio, float nearPlane, float farPlane);

  chr::gl::Matrix getModelViewMatrix();
  glm::mat4 getProjectionMatrix();

protected:
  glm::vec3 eyePoint;
  glm::vec3 viewDirection;
  glm::quat orientation;

  float fovy;
  float aspectRatio;
  float nearPlane, farPlane;
};
