#include "common/SeaSurfaceSpiral1.h"

#include "chr/math/Utils.h"

using namespace std;
using namespace chr;
using namespace gl;

SeaSurfaceSpiral1::SeaSurfaceSpiral1()
:
vertices(vertexBuffer->storage),
colors(colorBuffer->storage)
{}

void SeaSurfaceSpiral1::setup(const SeaSurface &surface, float ox, float oy, float r1, float r2, float turns, float DD1, float DD2, float gray)
{
  points.clear();
  colors.clear();
  samples.clear();

  size = 0;

  float l = TWO_PI * turns;
  float L = PI * turns * (r1 + r2);
  float dr = (r2 - r1) / l;
  float DD = (DD2 - DD1) / l;
  float D = 0;
  
  do
  {
    float r = sqrtf(r1 * r1 + 2 * dr * D);
    float d = (r - r1) / dr;
    D += DD1 + d * DD;
    
    points.emplace_back(ox - sinf(d) * r, oy + cosf(d) * r);
    
    float alpha = gray * (1 - (r - r1) / (r2 - r1));
    colors.emplace_back(0, 0, 0, alpha);
    
    size++;
  }
  while (D < L);

  vertices.reserve(size);
  colorBuffer.requestUpload();

  // ---
  
  samples.reserve(size);
  
  for (auto i = 0; i < size; i++)
  {
    surface.sample(points[i], samples[i]);
  }
}

void SeaSurfaceSpiral1::update(const SeaSurface &surface, float t, float swellFactor)
{
  vertices.clear();
  vertexBuffer.requestUpload();

  for (auto i = 0; i < size; i++)
  {
    vertices.emplace_back(glm::vec3(points[i], 0) + surface.getDisplacement(samples[i], t, swellFactor));
  }
}

void SeaSurfaceSpiral1::draw(const ShaderProgram &shader)
{
  vertexBuffer.bind(shader);
  colorBuffer.bind(shader);

  glDrawArrays(GL_LINE_STRIP, 0, size);

  vertexBuffer.unbind(shader);
  colorBuffer.unbind(shader);
}

glm::vec3 SeaSurfaceSpiral1::getVertex(int index) const
{
  return vertices[index].position;
}

size_t SeaSurfaceSpiral1::vertexCount()
{
  return size;
}
