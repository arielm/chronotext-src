#pragma once

#include "chr/FileSystem.h"
#include "chr/gl/Buffer.h"

#include "common/xf/Font.h"

namespace chr
{
  namespace xf
  {
    struct FontData
    {
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

      FontData(int glyphCount);
      ~FontData();
    };

    struct FontAtlas
    {
      int width;
      int height;
      uint8_t *data;

      FontAtlas(int width, int height);
      ~FontAtlas();

      void addGlyph(const uint8_t *glyphData, int glyphX, int glyphY, int glyphWidth, int glyphHeight);
    };

    struct FontTexture
    {
        int width;
        int height;
        uint32_t glId = 0;
        InputSource inputSource;

        FontTexture(FontAtlas *atlas, const InputSource &inputSource, bool useAnisotropy = false);
        ~FontTexture();

        void upload(FontAtlas *atlas, bool useAnisotropy = false);
        void discard();
        bool reload();
        void bind();
        void unbind();

        size_t getMemoryUsage() const;
        glm::vec2 getSize() const;
    };

    class FontManager
    {
    public:
        std::shared_ptr<Font> getFont(const InputSource &inputSource, const Font::Properties &properties);

        FontManager();
        void shutdown();

        void unload(std::shared_ptr<Font> font);
        void unload(const InputSource &inputSource);
        void unload();

        void discardTextures();
        void reloadTextures();

        size_t getTextureMemoryUsage() const;

    protected:
        friend class Font;
        friend struct FontTexture;

        std::map<std::pair<std::string, Font::Properties>, std::shared_ptr<Font>> fonts;
        std::map<std::string, std::pair<std::unique_ptr<FontData>, std::unique_ptr<FontTexture>>> fontDataAndTextures;

        gl::Buffer<GLushort> indexBuffer;
        std::vector<GLushort> &indices;

        void discardUnusedTextures();

        static std::pair<FontData*, FontAtlas*> fetchFontDataAndAtlas(const InputSource &inputSource);
        gl::Buffer<GLushort>& getIndexBuffer(size_t capacity);
    };
  }
}
