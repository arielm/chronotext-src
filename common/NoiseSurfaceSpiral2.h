#pragma once

#include "chr/gl/Buffer.h"
#include "chr/path/SplinePath.h"
#include "chr/path/FollowablePath3D.h"

#include "common/NoiseSurface.h"

class NoiseSurfaceSpiral2
{
public:
  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> vertexBuffer;
  chr::path::FollowablePath3D path;

  NoiseSurfaceSpiral2();

  void setup(float r1, float r2, float turns, float DD1, float DD2);
  void update(const NoiseSurface &surface, float height, const glm::vec2 &offset = glm::vec2(0));

  void enableWire(bool enable);
  void enablePath(bool enable);

  void setDirection(float direction);
  void setSampleSize(float size);
  void setSamplingTolerance(float tolerance);

protected:
  bool wireEnabled = true;
  bool pathEnabled = false;

  float direction = 1;
  float sampleSize = 1;
  float samplingTolerance = 1;

  std::vector<glm::vec2> points;
  chr::path::SplinePath<glm::vec3> spline;
};
