#pragma once

#include "common/xf/FontSequence.h"

#include "chr/gl/Matrix.h"
#include "chr/gl/Buffer.h"

#include <map>
#include <string>

namespace chr
{
  typedef class xf::Font XFont;

  namespace xf
  {
    class FontManager;
    struct FontData;
    struct FontTexture;
    
    class Font
    {
    public:
      enum Alignment
      {
        ALIGN_MIDDLE,
        ALIGN_LEFT,
        ALIGN_RIGHT,
        ALIGN_TOP,
        ALIGN_BASELINE,
        ALIGN_BOTTOM
      };
      
      struct Properties
      {
        bool useAnisotropy;
        int slotCapacity;
        
        Properties(bool useAnisotropy, int slotCapacity)
        :
        useAnisotropy(useAnisotropy),
        slotCapacity(slotCapacity)
        {}
        
        Properties& setCapacity(int slotCapacity)
				{
          Properties::slotCapacity = slotCapacity;
          return *this;
        }
        
        bool operator<(const Properties &rhs) const
        {
          return std::tie(useAnisotropy, slotCapacity) < std::tie(rhs.useAnisotropy, rhs.slotCapacity);
        }
      };
      
      static Properties Properties2d()
      {
        return Properties(false, 4096);
      }
      
      static Properties Properties3d()
      {
        return Properties(true, 4096);
      }
      
      bool isSpace(char16_t c) const;
      bool isValid(char16_t c) const;
      int getGlyphIndex(char16_t c) const;
      std::u16string getCharacters() const;

      void setSize(float size);
      void setMiddleLineFactor(float factor); // DEFAULT-VALUE IS 0, OTHERWISE getOffsetY() FOR "ALIGN_MIDDLE" WILL RETURN middleLineFactor * (getAscent() - getDescent())
      void setDirection(float direction);
      void setAxis(const glm::vec2 &axis);
      inline void setAxis(float x, float y) { setAxis(glm::vec2(x, y)); }

      void setShader(const gl::ShaderProgram &shader);
      void setColor(const glm::vec4 &color);
      void setColor(float r, float g, float b, float a);

      void setClip(const math::Rectf &clipRect);
      void setClip(float x1, float y1, float x2, float y2);
      void clearClip(); // INVOKED UPON SEQUENCE-BEGIN
      
      float getSize() const;
      float getDirection() const;
      const glm::vec2& getAxis() const;
      
      float getGlyphAdvance(int glyphIndex) const;
      float getCharAdvance(char16_t c) const;
      float getStringAdvance(const std::u16string &s) const;
      float getSubStringAdvance(const std::u16string &s, int begin, int end) const;
      
      float getHeight() const;
      float getAscent() const;
      float getDescent() const;
      float getLineThickness() const;
      float getUnderlineOffset() const;
      float getStrikethroughOffset() const;

      float getOffsetX(char16_t c, Alignment align) const;
      float getOffsetX(const std::u16string &text, Alignment align) const;
      float getOffsetY(Alignment align) const; // FOR "ALIGN_MIDDLE": getStrikethroughOffset() WILL BE USED, UNLESS setMiddleLineFactor() HAS BEEN INVOKED
      inline glm::vec2 getOffset(const std::u16string &text, Alignment alignX, Alignment alignY) const { return glm::vec2(getOffsetX(text, alignX), getOffsetY(alignY)); }

      void beginSequence(FontSequence &sequence, bool useColor = false);
      void endSequence();
      void replaySequence(FontSequence &sequence);

      void addGlyph(int glyphIndex, float x, float y, float z = 0);
      void addGlyph(const gl::Matrix &matrix, int glyphIndex, float x = 0, float y = 0);

      Font(FontManager &fontManager, FontData *data, FontTexture *texture, const Properties &properties); // FIXME: SHOULD BE PROTECTED

    protected:
      friend class FontManager;

      int glyphCount;
      std::map<char16_t, int> glyphs;
      
      float baseSize;
      float height;
      float ascent;
      float descent;
      float lineThickness;
      float underlineOffset;
      float strikethroughOffset;
      float spaceAdvance;
      
      float *w;
      float *h;
      float *le;
      float *te;
      float *advance;
      
      float *u1;
      float *v1;
      float *u2;
      float *v2;
      
      FontTexture *texture;

      Properties properties;
      gl::Buffer<GLushort> indexBuffer;

      float size;
      float sizeRatio;
      float middleLineFactor = 0;
      float direction;
      glm::vec2 axis;

      ShaderProgram shader;
      bool hasShader = false;
      glm::vec4 color;
      
      bool hasClip = false;
      math::Rectf clipRect;
      
      int began = 0;
      bool sequenceUseColor;
      FontSequence *sequence = nullptr;
      std::unique_ptr<gl::QuadBatch> batch;

      void incrementSequence();
      bool fillQuad(gl::Quad<gl::XYZ.UV> &quad, int glyphIndex, float x, float y) const;
      bool clipQuad(gl::Quad<gl::XYZ.UV> &quad) const;
    };
  }
}
