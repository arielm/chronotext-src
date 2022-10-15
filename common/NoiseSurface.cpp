#include "common/NoiseSurface.h"

#include "chr/math/Utils.h"

using namespace std;
using namespace chr;

NoiseSurface::NoiseSurface(int gridWidth, int gridSize, float noiseScale, int noiseOctaves, int64_t noiseSeed)
:
gridWidth(gridWidth),
gridSize(gridSize),
noiseScale(noiseScale),
noise(noiseSeed, noiseOctaves)
{
    n = gridWidth / gridSize;
    grid = new float[n * n];

    baseFunction = bind(&NoiseSurface::tilingNoise2d, this, std::placeholders::_1, std::placeholders::_2, noiseScale);
}

NoiseSurface::~NoiseSurface()
{
    delete[] grid;
}

float NoiseSurface::getHeight(float x, float y) const
{
    x = math::boundf(x, gridWidth) / gridSize;
    y = math::boundf(y, gridWidth) / gridSize;
    
    int gx = (int)x;
    float xfrac = x - gx;
    
    int gy = (int)y;
    float yfrac = y - gy;
    
    int gx1 = (gx + 1) % n;
    int gy1 = (gy + 1) % n;
    float s00 = grid[gx + gy * n];
    float s01 = grid[gx1 + gy * n];
    float s10 = grid[gx + gy1 * n];
    float s11 = grid[gx1 + gy1 * n];
    
    float phi0 = (s01 - s00) * xfrac + s00;
    float phi1 = (s11 - s10) * xfrac + s10;
    return (phi1 - phi0) * yfrac + phi0;
}

float NoiseSurface::tilingNoise1d(float x, float scale) const
{
    float u = x / gridWidth;
    return u * noise.get(x * scale) + (1.0f - u) * noise.get((x + gridWidth) * scale);
}

float NoiseSurface::tilingNoise2d(float x, float y, float scale) const
{
    float u = x / gridWidth;
    float v = y / gridWidth;
    float u1 = 1.0f - u;
    float v1 = 1.0f - v;
    
    float x1 = (x + gridWidth) * scale;
    float y1 = (y + gridWidth) * scale;
    x *= scale;
    y *= scale;
    
    return u * v * noise.get(x, y) + u * v1 * noise.get(x, y1) + u1 * v * noise.get(x1, y) + u1 * v1 * noise.get(x1, y1);
}

void NoiseSurface::setBaseFunction(const function<float (float, float)> &baseFunction)
{
    NoiseSurface::baseFunction = baseFunction;
}

void NoiseSurface::generateBase()
{
    noiseMin = numeric_limits<float>::max();
    noiseMax = numeric_limits<float>::min();
    
    for (int iy = 0; iy < n; iy++)
    {
        for (int ix = 0; ix < n; ix++)
        {
            float value = baseFunction(ix * gridSize, iy * gridSize);
            grid[ix + iy * n] = value;
            
            if (value > noiseMax) noiseMax = value;
            if (value < noiseMin) noiseMin = value;
        }
    }
}

void NoiseSurface::reseed(int64_t seed)
{
    noise.reseed(seed);
    generateBase();
}