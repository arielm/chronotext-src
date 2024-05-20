#pragma once

#include "common/SeaSurface.h"

#include "chr/gl/Buffer.h"

#include <vector>

class SeaSurfaceSpiral1
{
public:
  chr::gl::Buffer<chr::gl::Vertex<chr::gl::XYZ>> vertexBuffer;
  std::vector<chr::gl::Vertex<chr::gl::XYZ>> &vertices;

  chr::gl::Buffer<chr::gl::Vertex<chr::gl::RGBA>> colorBuffer;
  std::vector<chr::gl::Vertex<chr::gl::RGBA>> &colors;

  SeaSurfaceSpiral1();

  void setup(const SeaSurface &surface, float ox, float oy, float r1, float r2, float turns, float DD1, float DD2, float gray);
  void update(const SeaSurface &surface, float t, float swellFactor = 1);
  void draw(const chr::gl::ShaderProgram &shader);

  glm::vec3 getVertex(int index) const;
  size_t vertexCount();
    
protected:
  size_t size;
  std::vector<glm::vec2> points;
  std::vector<SeaSurface::Sample> samples;
};
