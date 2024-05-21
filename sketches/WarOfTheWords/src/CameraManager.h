#pragma once

#include "chr/gl/Matrix.h"
#include "chr/path/FollowablePath2D.h"

class CameraManager
{
public:
  chr::path::FollowablePath2D path;
  float offset = 0;

  void resize(const glm::ivec2 &size);
  void run(float elapsed);
  void reset();

  glm::mat4 getMVPMatrix() const;
  bool hasReachedEnd(float fadeDuration);

protected:
  glm::vec3 viewPosition;
  glm::quat viewOrientation;

  glm::mat4 projectionMatrix;
  glm::mat4 mvpMatrix;

  glm::quat getViewOrientation(chr::gl::Matrix &matrix);
};
