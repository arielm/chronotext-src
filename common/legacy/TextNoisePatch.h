#pragma once

#include "NoisePatch.h"

#include "chr/math/Rect.h"

#include "common/LineProvider.h"
#include "common/legacy/HPath.h"

class TextNoisePatch : public NoisePatch
{
public:
  TextNoisePatch() = default;
  TextNoisePatch(NoiseSurface *surface, float polygonOffset, std::shared_ptr<chr::XFont> font, std::shared_ptr<LineProvider> lineProvider, float lineHeight);

  void drawLine(const HPath &path, float ox, float oy, int lineIndex, float xm, float xp, const chr::math::Rectf &clip = chr::math::Rectf(0, 0, 0, 0));
  float drawLine(int pathIndex, float xm, float xp, float ox, float oy, float d, const chr::math::Rectf &clip = chr::math::Rectf(0, 0, 0, 0));
  void drawText(float xm, float ym, float w, float h, float ox, float oy, float d);

protected:
  float polygonOffset;
  std::shared_ptr<chr::XFont> font;
  std::shared_ptr<LineProvider> lineProvider;
  float lineHeight;

  std::vector<HPath> paths;

  void compute(HPath &path, float modY, float d);
};
