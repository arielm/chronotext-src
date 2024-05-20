#pragma once

#include "chr/gl/Texture.h"
#include "chr/math/Rect.h"

class TiledAtlas
{
public:
  TiledAtlas() = default;
  TiledAtlas(fs::path relativePath, int titleSize, int padding = 0);

  chr::gl::Texture getTexture() const;
  chr::math::Rectf getTileBounds(int col, int row) const;

protected:
  int tileSize;
  int padding = 0;
  chr::gl::Texture texture;
};
