
#pragma once

#include "common/quad/QuadBatch.h"

#include <memory>

using namespace chr::gl;

namespace chr
{
  namespace xf
  {
    class Font;
    
    class FontSequence
    {
    public:
      bool useColor;
      
      FontSequence() = default;
      
      /*
       * NOT ALLOWING COPY, BECAUSE OF THE vector OF unique_ptr
       */
      FontSequence(const FontSequence &other) = delete;
      void operator=(const FontSequence &other) = delete;
      
    protected:
      std::vector<std::unique_ptr<gl::QuadBatch>> batches;
      
      void begin(bool useColor = false);
      void end();
      
      void addBatch(std::unique_ptr<gl::QuadBatch> &&batch);
      void replay(const gl::ShaderProgram &shader, Buffer<GLushort> &indexBuffer);
      
      friend class Font;
    };
  }
}
