#include "Camera.h"

using namespace chr;
using namespace gl;

void Camera::setEyePoint(const glm::vec3 &eyePoint)
{
  this->eyePoint = eyePoint;
}

void Camera::setOrientation(const glm::vec3 &axis, float radians)
{
  float w = cosf(radians / 2);
  glm::vec3 v = glm::normalize(axis) * sinf(radians / 2);
  glm::quat q(w, v);

  orientation = glm::normalize(q);
  viewDirection = glm::rotate(orientation, glm::vec3(0, 0, -1));
}

void Camera::setPerspective(float fovy, float aspectRatio, float nearPlane, float farPlane)
{
  this->fovy = fovy;
  this->aspectRatio = aspectRatio;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;
}

chr::gl::Matrix Camera::getModelViewMatrix()
{
  auto mW = -glm::normalize(viewDirection);
  auto mU = glm::rotate( orientation, glm::vec3(1, 0, 0));
  auto mV = glm::rotate( orientation, glm::vec3(0, 1, 0));

  glm::vec3 d(-glm::dot(eyePoint, mU), -glm::dot(eyePoint, mV), -glm::dot(eyePoint, mW));

  glm::mat4 m;
  m[0][0] = mU.x; m[1][0] = mU.y; m[2][0] = mU.z; m[3][0] =  d.x;
  m[0][1] = mV.x; m[1][1] = mV.y; m[2][1] = mV.z; m[3][1] =  d.y;
  m[0][2] = mW.x; m[1][2] = mW.y; m[2][2] = mW.z; m[3][2] =  d.z;
  m[0][3] = 0.0f; m[1][3] = 0.0f; m[2][3] = 0.0f; m[3][3] = 1.0f;

  return Matrix(m);
}

glm::mat4 Camera::getProjectionMatrix()
{
  return glm::perspective(fovy, aspectRatio, nearPlane, farPlane);
}
