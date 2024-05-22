#pragma once

#include "WaveSurface.h"

#include "chr/path/FollowablePath3D.h"

class WaveSurfaceLine
{
public:
  WaveSurface *wave;
  float y;
  float width;
  float segmentLength;
  float sampleSize;

  int size;
  chr::path::FollowablePath3D path;

  WaveSurfaceLine(WaveSurface *wave, float y, float width, float segmentLength, float sampleSize);

  void compute(float t);
};
