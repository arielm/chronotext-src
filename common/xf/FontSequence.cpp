
#include "common/xf/FontSequence.h"

using namespace std;
using namespace chr::gl;

namespace chr
{
  namespace xf
  {
    void FontSequence::begin(bool useColor)
    {
      this->useColor = useColor;
      batches.clear();
    }
    
    void FontSequence::end()
    {}
    
    void FontSequence::addBatch(unique_ptr<QuadBatch> &&batch)
    {
      batches.emplace_back(forward<unique_ptr<QuadBatch>>(batch));
    }
    
    void FontSequence::replay(const ShaderProgram &shader, Buffer<GLushort> &indexBuffer)
    {
      for (auto &batch : batches)
      {
        batch->flush(shader, indexBuffer, useColor);
      }

      if (useColor)
      {
        ShaderHelper::unbindAttributes<Vertex<XYZ.RGBA>>(shader);
      }

      ShaderHelper::unbindAttributes<Vertex<XYZ.UV>>(shader);
    }
  }
}
