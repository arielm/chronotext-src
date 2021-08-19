#pragma once

#include "chr/gl/TextureManager.h"

#include "common/stroke/StrokeHelper.h"

namespace chr
{
  class Hairline
  {
  public:
    enum Type
    {
      TYPE_NORMAL,
      TYPE_DASHED
    };

    chr::gl::VertexBatch<chr::gl::XYZ.UV> strip;

    Hairline() = default;
    Hairline(gl::TextureManager &textureManager, Type type = TYPE_NORMAL);
    
    void stroke(const path::FollowablePath2D &path, float scale = 1, float uOffset = 0);
    void stroke(const path::FollowablePath2D &path, const std::vector<float> &offsets, float scale = 1, float uOffset = 0);
    void clear();

  protected:
    Type type;
  };
}
