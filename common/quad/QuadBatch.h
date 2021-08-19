#pragma once

#include "chr/gl/Matrix.h"
#include "chr/gl/Buffer.h"

namespace chr
{
  namespace gl
  {
    class QuadBatch
    {
    public:
      Buffer<Vertex<XYZ.UV>> vertexBuffer;
      Buffer<Vertex<RGBA>> colorBuffer;

      std::vector<Vertex<XYZ.UV>> &vertices;
      std::vector<Vertex<RGBA>> &colors;

      QuadBatch()
      :
      vertices(vertexBuffer->storage),
      colors(colorBuffer->storage)
      {}

      void clear();
      void pack();
      void flush(const ShaderProgram &shader, Buffer<GLushort> &indexBuffer, bool useColor = false);

      inline int size() const
      {
        return vertices.size() >> 2;
      }

      inline bool empty() const
      {
        return vertices.empty();
      }

      inline void addQuad(const Matrix &matrix, const Quad<XYZ.UV> &quad)
      {
        matrix.addTransformedQuad(quad, vertices);
      }

      inline void addQuad(const Quad<XYZ.UV> &quad, float z = 0)
      {
        vertices.emplace_back(quad.x1, quad.y1, z, quad.u1, quad.v1);
        vertices.emplace_back(quad.x2, quad.y1, z, quad.u2, quad.v1);
        vertices.emplace_back(quad.x2, quad.y2, z, quad.u2, quad.v2);
        vertices.emplace_back(quad.x1, quad.y2, z, quad.u1, quad.v2);
      }

      inline void addColor(const glm::vec4 &color)
      {
        colors.emplace_back(color);
        colors.emplace_back(color);
        colors.emplace_back(color);
        colors.emplace_back(color);
      }
    };
  }
}
