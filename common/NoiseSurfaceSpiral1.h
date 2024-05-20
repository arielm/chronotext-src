#pragma once

#include "chr/gl/Buffer.h"
#include "chr/path/FollowablePath3D.h"

#include "common/NoiseSurface.h"

class NoiseSurfaceSpiral1
{
public:
  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> vertexBuffer;
  chr::path::FollowablePath3D path;

  NoiseSurfaceSpiral1() = default;

  void setup(float r1, float r2, float turns, float DD1, float DD2);
  void update(const NoiseSurface &surface, float height, const glm::vec2 &offset = glm::zero<glm::vec2>());

  void enableWire(bool enable);
  void enablePath(bool enable);

  void setDirection(float direction);
  void setSampleSize(float size);

protected:
  bool wireEnabled = true;
  bool pathEnabled = false;

  float direction = 1;
  float sampleSize;

  std::vector<glm::vec2> points;
  std::vector<glm::vec2> pointsM;
  std::vector<glm::vec2> pointsP;
};
