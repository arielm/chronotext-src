#include "common/quad/QuadBatch.h"

namespace chr
{
  namespace gl
  {
    void QuadBatch::clear()
    {
      vertexBuffer.clear();
      colorBuffer.clear();
    }

    void QuadBatch::pack()
    {
      vertices.shrink_to_fit();
      colors.shrink_to_fit();
    }

    void QuadBatch::flush(const ShaderProgram &shaderProgram, Buffer<GLushort> &indexBuffer, bool useColor)
    {
      if (!empty())
      {
        vertexBuffer.bind(shaderProgram);

        if (useColor)
        {
          colorBuffer.bind(shaderProgram);
        }

        indexBuffer.bind(shaderProgram);
        indexBuffer.draw(GL_TRIANGLES, size() * 6);

        vertexBuffer.unbind(shaderProgram);

        if (useColor)
        {
          colorBuffer.unbind(shaderProgram);
        }
      }
    }
  }
}
