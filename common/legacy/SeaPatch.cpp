#include "SeaPatch.h"

using namespace std;
using namespace chr;

SeaPatch::SeaPatch(SeaSurface *surface)
:
surface(surface),
gridBatch(GL_LINES, vertexBuffer, gridIndexBuffer),
fillBatch(GL_TRIANGLES, vertexBuffer, fillIndexBuffer)
{}

SeaPatch::~SeaPatch()
{
  delete[] samples;
}

void SeaPatch::setFrontFace(GLenum mode)
{
  frontFace = mode;
}

void SeaPatch::setup(float ox, float oy, float w, float h, int gridSize, int mode)
{
  this->ox = ox;
  this->oy = oy;
  this->gridSize = gridSize;

  nx = w / gridSize;
  ny = h / gridSize;

  // ---

  samples = new SeaSurface::Sample[nx * ny];

  for (int iy = 0; iy < ny; iy++)
  {
    float yy = oy + iy * gridSize;

    for (int ix = 0; ix < nx; ix++)
    {
      float xx = ox + ix * gridSize;
      int i = ix + iy * nx;
      surface->sample(xx, yy, samples[i]);
    }
  }

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

void SeaPatch::update(float t, float swellFactor)
{
  auto &vertices = vertexBuffer->storage;
  vertices.clear();
  vertices.reserve(nx * ny);
  vertexBuffer.requestUpload();

  for (int iy = 0; iy < ny; iy++)
  {
    float yy = oy + iy * gridSize;
    for (int ix = 0; ix < nx; ix++)
    {
      float xx = ox + ix * gridSize;
      int i = ix + iy * nx;
      vertices.push_back(glm::vec3(xx, yy, 0) + surface->getDisplacement(samples[i], t, swellFactor));
    }
  }
}

void SeaPatch::drawFill(const glm::vec4 &color)
{
  fillBatch
    .setShaderColor(color)
    .flush();
}

void SeaPatch::drawGrid(const glm::vec4 &color)
{
  gridBatch
    .setShaderColor(color)
    .flush();
}
