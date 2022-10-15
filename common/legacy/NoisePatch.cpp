#include "NoisePatch.h"

using namespace std;
using namespace chr;

NoisePatch::NoisePatch(NoiseSurface *surface)
:
surface(surface),
gridBatch(GL_LINES, vertexBuffer, gridIndexBuffer),
fillBatch(GL_TRIANGLES, vertexBuffer, fillIndexBuffer)
{}

void NoisePatch::setFrontFace(GLenum mode)
{
  frontFace = mode;
}

void NoisePatch::setup(float cx, float cy, float w, float h, int mode)
{
  ox1 = floorf(cx / surface->gridSize);
  ox2 = ceilf((cx + w) / surface->gridSize);

  oy1 = floorf(cy / surface->gridSize);
  oy2 = ceilf((cy + h) / surface->gridSize);

  nx = int(ox2 - ox1) + 1;
  ny = int(oy2 - oy1) + 1;

  // ---

  auto &gridIndices = gridIndexBuffer->storage;
  gridIndices.clear();
  gridIndices.reserve(ny * (nx - 1) * 2 * ((mode & MODE_GRID_H) ? 1 : 0) + nx * (ny - 1) * 2 * ((mode & MODE_GRID_V) ? 1 : 0));

  if (mode & MODE_GRID_H)
  {
    for (int iy = 0; iy < ny; iy++)
    {
      for (int ix = 0; ix < nx - 1; ix++)
      {
        gridIndices.push_back(iy * nx + ix);
        gridIndices.push_back(iy * nx + ix + 1);
      }
    }
  }

  if (mode & MODE_GRID_V)
  {
    for (int ix = 0; ix < nx; ix++)
    {
      for (int iy = 0; iy < ny - 1; iy++)
      {
        gridIndices.push_back(iy * nx + ix);
        gridIndices.push_back((iy + 1) * nx + ix);
      }
    }
  }

  if ((mode & MODE_GRID_H) || (mode & MODE_GRID_V))
  {
    gridIndexBuffer.requestUpload();
  }

  // ---

  if (mode & MODE_FILL)
  {
    auto &fillIndices = fillIndexBuffer->storage;
    fillIndices.clear();
    fillIndices.reserve((nx - 1) * (ny - 1) * 6);
    fillIndexBuffer.requestUpload();

    for (int iy = 0; iy < ny - 1; iy++)
    {
      for (int ix = 0; ix < nx - 1; ix++)
      {
        if (frontFace == CCW)
        {
          fillIndices.push_back(iy * nx + ix);
          fillIndices.push_back(iy * nx + ix + 1);
          fillIndices.push_back((iy + 1) * nx + ix + 1);

          fillIndices.push_back((iy + 1) * nx + ix + 1);
          fillIndices.push_back((iy + 1) * nx + ix);
          fillIndices.push_back(iy * nx + ix);
        }
        else
        {
          fillIndices.push_back(iy * nx + ix);
          fillIndices.push_back((iy + 1) * nx + ix);
          fillIndices.push_back((iy + 1) * nx + ix + 1);

          fillIndices.push_back((iy + 1) * nx + ix + 1);
          fillIndices.push_back(iy * nx + ix + 1);
          fillIndices.push_back(iy * nx + ix);
        }
      }
    }
  }
}

void NoisePatch::update(float height, const glm::vec2 &offset)
{
  auto &vertices = vertexBuffer->storage;
  vertices.clear();
  vertices.reserve(nx * ny);
  vertexBuffer.requestUpload();

  for (int iy = 0; iy < ny; iy++)
  {
    float y = (oy1 + iy) * surface->gridSize;
    for (int ix = 0; ix < nx; ix++)
    {
      float x = (ox1 + ix) * surface->gridSize;
      vertices.emplace_back(x, y, height * surface->getHeight(x + offset.x, y + offset.y));
    }
  }
}

void NoisePatch::drawFill(const glm::vec4 &color)
{
  fillBatch
    .setShaderColor(color)
    .flush();
}

void NoisePatch::drawGrid(const glm::vec4 &color)
{
  gridBatch
    .setShaderColor(color)
    .flush();
}
