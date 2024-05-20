#include "common/legacy/ui/TiledAtlas.h"

using namespace chr;
using namespace gl;
using namespace math;

TiledAtlas::TiledAtlas(fs::path relativePath, int tileSize, int padding)
:
tileSize(tileSize),
padding(padding)
{
  texture = Texture(
    Texture::ImageRequest(relativePath)
      .setFlags(image::FLAGS_TRANSLUCENT | image::FLAGS_POT)
      .setMipmap(true));
}

Texture TiledAtlas::getTexture() const
{
  return texture;
}

Rectf TiledAtlas::getTileBounds(int col, int row) const
{
  return Rectf(padding + col * (tileSize + padding), padding + row * (tileSize + padding), tileSize, tileSize);
}
