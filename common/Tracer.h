#pragma once

#include "chr/path/FollowablePath2D.h"

class Tracer
{
public:
  chr::path::FollowablePath2D rawTrace;
  chr::path::FollowablePath2D path;

  Tracer() = default;
  Tracer(float chunkLength, float tol = 1);
  
  void begin();
  void end();
  void update(const glm::vec2 &point);

  void trace(const chr::path::FollowablePath2D &input);

protected:
  float chunkLength = 0;
  float tol = 1;
};
