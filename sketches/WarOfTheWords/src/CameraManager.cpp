#include "CameraManager.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;

static constexpr float CAMERA_FOVY = 45;
static constexpr float CAMERA_ZNEAR = 1;
static constexpr float CAMERA_ZFAR = 3000;
static constexpr float CAMERA_ELEVATION = 315;
static constexpr float CAMERA_AZIMUTH = 0;
static constexpr float CAMERA_DISTANCE = 350;

static constexpr float CAMERA_ROTATION_INTERP = 0.03f;
static constexpr float CAMERA_ROTATION_FACTOR = 0.2f; // 0: NO PATH-FOLLOWING; 1: FULL-PATH FOLLOWING
static constexpr float CAMERA_MOTION_INTERP = 0.1f;
static constexpr float CAMERA_VELOCITY = 20;

void CameraManager::resize(const glm::ivec2 &size)
{
  projectionMatrix = glm::perspective(CAMERA_FOVY * D2R, size.x / (float)size.y, CAMERA_ZNEAR, CAMERA_ZFAR);
}

void CameraManager::run(float elapsed)
{
  const auto value = path.offsetToValue(offset);

  Matrix matrix;
  value.applyToMatrix(matrix);

  if (offset == 0)
  {
    viewOrientation = getViewOrientation(matrix);
    viewPosition = glm::vec3(value.position, 0);
  }
  else
  {
    glm::quat q1 = getViewOrientation(matrix);
    glm::quat q2 = glm::slerp(viewOrientation, q1, CAMERA_ROTATION_INTERP);
    viewOrientation = glm::normalize(viewOrientation + q2);

    viewPosition += (glm::vec3(value.position, 0) - viewPosition) * CAMERA_MOTION_INTERP;
  }

  offset = CAMERA_VELOCITY * elapsed;

  // ---

  Matrix modelViewMatrix;
  modelViewMatrix
    .scale(1, -1, 1)
    .translate(0, 0, -CAMERA_DISTANCE)
    .rotateX(-CAMERA_ELEVATION * D2R)
    .rotateZ(-CAMERA_AZIMUTH * D2R)
    .applyQuat<-1>(viewOrientation)
    .translate(-viewPosition);

  mvpMatrix = modelViewMatrix * projectionMatrix;
}

void CameraManager::reset()
{
  offset = 0;
}

glm::quat CameraManager::getViewOrientation(Matrix &matrix)
{
  return glm::slerp(glm::quat(), matrix.getQuat(), CAMERA_ROTATION_FACTOR);
}

glm::mat4 CameraManager::getMVPMatrix() const
{
  return mvpMatrix;
}

bool CameraManager::hasReachedEnd(float fadeDuration)
{
  return (offset > path.getLength() - fadeDuration / CAMERA_VELOCITY);
}
