#include "common/stroke/Hairline.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;

namespace chr
{
  Hairline::Hairline(TextureManager &textureManager, Type type)
  :
  type(type)
  {
    string resourceName;
    
    switch (type)
    {
      case TYPE_NORMAL:
        resourceName = "hairline.png";
        break;
        
      case TYPE_DASHED:
        resourceName = "hairline_dashed.png";
        break;
        
      default:
        assert(false);
    }
    
    strip
      .setTexture(textureManager.get(
        Texture::ImageRequest(resourceName)
          .setFlags(image::FLAGS_TRANSLUCENT)
          .setWrap(GL_REPEAT, GL_CLAMP_TO_EDGE)))
      .setPrimitive(GL_TRIANGLE_STRIP);
  }
  
  void Hairline::stroke(const FollowablePath2D &path, float scale, float uOffset)
  {
    StrokeHelper::stroke(path, strip, 4 / scale, (type == TYPE_NORMAL) ? 1 : 0.5f, uOffset);
  }
  
  void Hairline::stroke(const FollowablePath2D &path, const vector<float> &offsets, float scale, float uOffset)
  {
    int size = offsets.size();
    
    if (size % 2 == 0)
    {
      strip.clear();
      
      for (int i = 0; i < size / 2; i++)
      {
        float offsetBegin = offsets[i * 2];
        float offsetEnd = offsets[i * 2 + 1];
        
        StrokeHelper::stroke(path, offsetBegin, offsetEnd, strip, 4 / scale, (type == TYPE_NORMAL) ? 1 : 0.5f, uOffset);
      }
    }
  }

  void Hairline::clear()
  {
      strip.clear();
  }
}
