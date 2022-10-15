/*
 * BASED ON PROCESSING'S IMPLEMENTATION OF PERLIN NOISE, CIRCA 2006
 *
 * THE LATEST (BUT SOMEWHAT LESS COMPLETE) VERSION CAN BE FOUND HERE:
 * https://github.com/processing/processing/blob/c9f116f8b5/core/src/processing/core/PApplet.java#L5317-L5538
 */
 
#pragma once

#include "common/JavaRandom.h"

class Noise
{
public:
    Noise(int octaves = 4, float falloff = 0.5f);
    Noise(int64_t seed, int octaves = 4, float falloff = 0.5f);
    
    ~Noise();

    void setOctaves(int octaves);
    void setFalloff(float falloff);
    void reseed(int64_t seed);

    float get(float x) const;
    float get(float x, float y) const;
    float get(float x, float y, float z) const;

protected:
    int octaves;
    float falloff;
    float* perlin;
    
    void generate(JavaRandom &rnd);
};
