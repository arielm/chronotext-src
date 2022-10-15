#pragma once

#include "common/Noise.h"

#include "chr/glm.h"

#include <functional>

class NoiseSurface
{
public:
    int gridWidth;
    int gridSize;
    float noiseScale;

    float noiseMin;
    float noiseMax;

    NoiseSurface(int gridWidth, int gridSize, float noiseScale, int noiseOctaves, int64_t noiseSeed);
    ~NoiseSurface();

    float getHeight(float x, float y) const; // 2x2 BILINEAR INTERPOLATION
    inline float getHeight(const glm::vec2 &p) const { return getHeight(p.x, p.y); }
    
    float tilingNoise1d(float x, float scale) const;
    float tilingNoise2d(float x, float y, float scale) const;
    
    void setBaseFunction(const std::function<float (float, float)> &baseFunction);
    void generateBase();
    void reseed(int64_t seed);

protected:
    Noise noise;
    
    int n;
    float *grid;
    
    std::function<float (float, float)> baseFunction;
};
