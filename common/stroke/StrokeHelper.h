#pragma once

#include "chr/gl/Batch.h"
#include "chr/path/FollowablePath2D.h"
#include "chr/utils/Utils.h"

namespace chr
{
  class StrokeHelper
  {
  public:
    /*
     * width: WIDTH OF THE STROKE (MAPPED TO THE HEIGHT OF THE TEXTURE)
     * uScale: RATIO BETWEEN THE HEIGHT AND WIDTH OF THE TEXTURE
     * uOffset: HORIZONTAL SHIFT OF THE TEXTURE, IN PIXELS
     *
     * CREATES A SINGLE-PORTION STRIP:
     * THE VertexBatch<XYZ.UV> IS AUTOMATICALLY CLEARED
     */
    static void stroke(const path::FollowablePath2D &path, chr::gl::VertexBatch<chr::gl::XYZ.UV> &strip, float width, float uScale = 1, float uOffset = 0)
    {
      auto size = path.size();
      
      if (size > 1)
      {
        strip.clear();
        strip.extendVertexCapacity(size * 2);
        auto &vertices = strip.vertices();

        const auto &points = path.getPoints();
        const auto &lengths = path.getLengths();
        
        float halfWidth = width * 0.5f;
        glm::vec2 w1(+halfWidth, -halfWidth);
        glm::vec2 w2(-halfWidth, +halfWidth);
        
        float uFreq = uScale / width;
        
        for (int i = 0; i < size; i++)
        {
          int o1, o2;
          
          if (i == size - 1)
          {
            o1 = 0;
            o2 = -1;
          }
          else
          {
            o1 = 1;
            o2 = 0;
          }
          
          float length = lengths[i + o1] - lengths[i + o2];
          glm::vec2 delta = (points[i + o1].position - points[i + o2].position) / length;
          float u = uFreq * (lengths[i] - uOffset);
          
          vertices.emplace_back(points[i].position + w1 * delta.yx(), u, 0);
          vertices.emplace_back(points[i].position + w2 * delta.yx(), u, 1);
        }
      }
    }
    
    /*
     * offsetBegin: DISTANCE BETWEEN THE BEGINNING OF THE PATH AND THE BEGINNING OF THE STROKE, IN PIXELS
     * offsetEnd: DISTANCE BETWEEN THE BEGINNING OF THE PATH AND THE END OF THE STROKE, IN PIXELS
     * width: WIDTH OF THE STROKE (MAPPED TO THE HEIGHT OF THE TEXTURE)
     * uScale: RATIO BETWEEN THE HEIGHT AND WIDTH OF THE TEXTURE
     * uOffset: HORIZONTAL SHIFT OF THE TEXTURE, IN PIXELS
     * discontinuous: IF TRUE, THE STRIP WILL BE DEGENERATED (I.E. ZERO-AREA TRIANGLES WILL BE INSERTED BETWEEN PORTIONS)
     *
     * POSSIBILITY TO CREATE A MULTI-PORTION STRIP:
     * THE VertexBatch<XYZ.UV> SHOULD THEREFORE BE CLEARED MANUALLY
     */
    static int stroke(const path::FollowablePath2D &path, float offsetBegin, float offsetEnd, chr::gl::VertexBatch<chr::gl::XYZ.UV> &strip, float width, float uScale = 1, float uOffset = 0, bool discontinuous = true)
    {
      int originalSize = strip.vertices().size();
      
      auto valueBegin = offset2Value(path, offsetBegin);
      auto valueEnd = offset2Value(path, offsetEnd);
      
      if (valueEnd.offset > valueBegin.offset)
      {
        auto &vertices = strip.vertices();
        const auto &points = path.getPoints();
        const auto &lengths = path.getLengths();
        
        float halfWidth = width * 0.5f;
        glm::vec2 w1(+halfWidth, -halfWidth);
        glm::vec2 w2(-halfWidth, +halfWidth);
        
        float uFreq = uScale / width;
        
        float length = lengths[valueBegin.index + 1] - valueBegin.offset;
        glm::vec2 delta = (points[valueBegin.index + 1].position - valueBegin.position) / length;
        float u = uFreq * (valueBegin.offset - uOffset);
        
        if (discontinuous && !strip.empty())
        {
          vertices.emplace_back(vertices[vertices.size() - 2].position, 0, 0);
        }
        
        for (int j = 0; j < ((discontinuous && !strip.empty()) ? 2 : 1); j++)
        {
          vertices.emplace_back(valueBegin.position + w1 * delta.yx(), u, 0);
        }
        
        vertices.emplace_back(valueBegin.position + w2 * delta.yx(), u, 1);

        for (int i = valueBegin.index + 1; i <= valueEnd.index; i++)
        {
          float length = lengths[i + 1] - lengths[i];
          glm::vec2 delta = (points[i + 1].position - points[i].position) / length;
          float u = uFreq * (lengths[i] - uOffset);
          
          vertices.emplace_back(points[i].position + w1 * delta.yx(), u, 0);
          vertices.emplace_back(points[i].position + w2 * delta.yx(), u, 1);
        }
        
        length = lengths[valueEnd.index] - valueEnd.offset;
        delta = (points[valueEnd.index].position - valueEnd.position) / length;
        u = uFreq * (valueEnd.offset - uOffset);
        
        vertices.emplace_back(valueEnd.position + w1 * delta.yx(), u, 0);
        vertices.emplace_back(valueEnd.position + w2 * delta.yx(), u, 1);
      }
      
      return (strip.vertices().size() - originalSize) >> 1;
    }

    /*
     * offsetBegin: DISTANCE BETWEEN THE BEGINNING OF THE PATH AND THE BEGINNING OF THE STROKE, IN PIXELS
     * offsetEnd: DISTANCE BETWEEN THE BEGINNING OF THE PATH AND THE END OF THE STROKE, IN PIXELS
     * width: WIDTH OF THE STROKE (MAPPED TO THE HEIGHT OF THE TEXTURE)
     * uScale: RATIO BETWEEN THE HEIGHT AND WIDTH OF THE TEXTURE
     * uOffset: HORIZONTAL SHIFT OF THE TEXTURE, IN PIXELS
     * discontinuous: IF TRUE, THE STRIP WILL BE DEGENERATED (I.E. ZERO-AREA TRIANGLES WILL BE INSERTED BETWEEN PORTIONS)
     *
     * POSSIBILITY TO CREATE A MULTI-PORTION STRIP:
     * THE VertexBatch<XYZ.UV> SHOULD THEREFORE BE CLEARED MANUALLY
     */
    static int stroke(const path::FollowablePath2D &path, float offsetBegin, float offsetEnd, chr::gl::VertexBatch<chr::gl::XYZ.UV.RGBA> &strip, const glm::vec4 &color, float width, float uScale = 1, float uOffset = 0, bool discontinuous = true)
    {
      int originalSize = strip.vertices().size();

      auto valueBegin = offset2Value(path, offsetBegin);
      auto valueEnd = offset2Value(path, offsetEnd);

      if (valueEnd.offset > valueBegin.offset)
      {
        auto &vertices = strip.vertices();
        const auto &points = path.getPoints();
        const auto &lengths = path.getLengths();

        float halfWidth = width * 0.5f;
        glm::vec2 w1(+halfWidth, -halfWidth);
        glm::vec2 w2(-halfWidth, +halfWidth);

        float uFreq = uScale / width;

        float length = lengths[valueBegin.index + 1] - valueBegin.offset;
        glm::vec2 delta = (points[valueBegin.index + 1].position - valueBegin.position) / length;
        float u = uFreq * (valueBegin.offset - uOffset);

        if (discontinuous && !strip.empty())
        {
          vertices.emplace_back(vertices[vertices.size() - 2].position, glm::vec2(0, 0), color);
        }

        for (int j = 0; j < ((discontinuous && !strip.empty()) ? 2 : 1); j++)
        {
          vertices.emplace_back(valueBegin.position + w1 * delta.yx(), glm::vec2(u, 0), color);
        }

        vertices.emplace_back(valueBegin.position + w2 * delta.yx(), glm::vec2(u, 1), color);

        for (int i = valueBegin.index + 1; i <= valueEnd.index; i++)
        {
          float length = lengths[i + 1] - lengths[i];
          glm::vec2 delta = (points[i + 1].position - points[i].position) / length;
          float u = uFreq * (lengths[i] - uOffset);

          vertices.emplace_back(points[i].position + w1 * delta.yx(), glm::vec2(u, 0), color);
          vertices.emplace_back(points[i].position + w2 * delta.yx(), glm::vec2(u, 1), color);
        }

        length = lengths[valueEnd.index] - valueEnd.offset;
        delta = (points[valueEnd.index].position - valueEnd.position) / length;
        u = uFreq * (valueEnd.offset - uOffset);

        vertices.emplace_back(valueEnd.position + w1 * delta.yx(), glm::vec2(u, 0), color);
        vertices.emplace_back(valueEnd.position + w2 * delta.yx(), glm::vec2(u, 1), color);
      }

      return (strip.vertices().size() - originalSize) >> 1;
    }

    /*
     * width: WIDTH OF THE STROKE (MAPPED TO THE HEIGHT OF THE TEXTURE)
     * uScale: RATIO BETWEEN THE HEIGHT AND WIDTH OF THE TEXTURE
     * uOffset: HORIZONTAL SHIFT OF THE TEXTURE, IN PIXELS
     *
     * CREATES A SINGLE-PORTION STRIP:
     * THE VertexBatch<XYZ.UV> IS AUTOMATICALLY CLEARED
     */
    static void stroke(const std::vector<glm::vec2> &points, chr::gl::VertexBatch<chr::gl::XYZ.UV> &strip, float width, float uScale = 1, float uOffset = 0)
    {
      auto size = points.size();
      
      if (size > 1)
      {
        strip.clear();
        strip.extendVertexCapacity(size * 2);
        auto &vertices = strip.vertices();

        float halfWidth = width * 0.5f;
        glm::vec2 w1(+halfWidth, -halfWidth);
        glm::vec2 w2(-halfWidth, +halfWidth);
        
        float uFreq = uScale / width;
        float u = -uOffset * uFreq;
        
        auto p0 = points[0];
        auto p1 = points[1];
        
        if (p0 != p1)
        {
          glm::vec2 delta(p1 - p0);
          float length = glm::length(delta);
          delta /= length;
          
          u += uFreq * length;
          
          vertices.emplace_back(p0 + w1 * delta.yx(), u, 0);
          vertices.emplace_back(p0 + w2 * delta.yx(), u, 1);
        }
        
        for (int i = 1; i < size - 1; i++)
        {
          auto p0 = points[i - 1];
          auto p1 = points[i];
          auto p2 = points[i + 1];
          
          if (p1 != p2)
          {
            glm::vec2 delta = glm::normalize(p2 - p1);
            u += uFreq * glm::length(p1 - p0);
            
            vertices.emplace_back(p1 + w1 * delta.yx(), u, 0);
            vertices.emplace_back(p1 + w2 * delta.yx(), u, 1);
          }
        }
        
        p0 = points[size - 2];
        p1 = points[size - 1];
        
        if (p0 != p1)
        {
          glm::vec2 delta(p1 - p0);
          float length = glm::length(delta);
          delta /= length;
          
          u += uFreq * length;
          
          vertices.emplace_back(p1 + w1 * delta.yx(), u, 0);
          vertices.emplace_back(p1 + w2 * delta.yx(), u, 1);
        }
      }
    }
    
  protected:
    static path::FollowablePath2D::Value offset2Value(const path::FollowablePath2D &path, float offset)
    {
      const auto &points = path.getPoints();
      const auto &lengths = path.getLengths();
      float length = path.getLength();
      
      if (offset < 0)
      {
        offset = 0;
      }
      else if (offset > length)
      {
        offset = length;
      }
      
      int index = utils::search(lengths, offset, 1, lengths.size());
      const auto &p0 = points[index].position;
      const auto &p1 = points[index + 1].position;
      
      float ratio = (offset - lengths[index]) / (lengths[index + 1] - lengths[index]);

      path::FollowablePath2D::Value value;
      value.position = p0 + (p1 - p0) * ratio;
      value.offset = offset;
      value.index = index;
      
      return value;
    }
  };
}
