#include "common/NoiseSurfaceGrid.h"

using namespace std;
using namespace chr;

void NoiseSurfaceGrid::setup(const NoiseSurface &surface, float cx, float cy, float w, float h)
{
  ox1 = floorf(cx / surface.gridSize);
  ox2 = ceilf((cx + w) / surface.gridSize);

  oy1 = floorf(cy / surface.gridSize);
  oy2 = ceilf((cy + h) / surface.gridSize);

  nx = int(ox2 - ox1) + 1;
  ny = int(oy2 - oy1) + 1;

  // ---

  auto &indices = indexBuffer->storage;
  indices.clear();
  indices.reserve(ny * (nx - 1) * 2 + nx * (ny - 1) * 2);
  indexBuffer.requestUpload();

  for (int iy = 0; iy < ny; iy++)
  {
    for (int ix = 0; ix < nx - 1; ix++)
    {
      indices.push_back(iy * nx + ix);
      indices.push_back(iy * nx + ix + 1);
    }
  }

  for (int ix = 0; ix < nx; ix++)
  {
    for (int iy = 0; iy < ny - 1; iy++)
    {
      indices.push_back(iy * nx + ix);
      indices.push_back((iy + 1) * nx + ix);
    }
  }
}

void NoiseSurfaceGrid::update(const NoiseSurface &surface, float height, const glm::vec2 &offset)
{
  auto &vertices = vertexBuffer->storage;
  vertices.clear();
  vertices.reserve(nx * ny);
  vertexBuffer.requestUpload();

  for (int iy = 0; iy < ny; iy++)
  {
    float y = (oy1 + iy) * surface.gridSize;

    for (int ix = 0; ix < nx; ix++)
    {
      float x = (ox1 + ix) * surface.gridSize;
      vertices.emplace_back(x, y, height * surface.getHeight(x + offset.x, y + offset.y));
    }
  }
}

size_t NoiseSurfaceGrid::vertexCount()
{
  return nx * ny;
}

size_t NoiseSurfaceGrid::indexCount()
{
  return nx * ny;
}
