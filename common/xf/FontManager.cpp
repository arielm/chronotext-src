#include "common/xf/FontManager.h"

#include "chr/io/BinaryInputStream.h"
#include "chr/Log.h"
#include "chr/gl/Utils.h"

#include <set>

using namespace std;
using namespace chr;
using namespace gl;
using namespace io;

namespace chr
{
  namespace xf
  {
    FontData::FontData(int glyphCount)
    :
    glyphCount(glyphCount)
    {
      w = new float[glyphCount];
      h = new float[glyphCount];
      le = new float[glyphCount];
      te = new float[glyphCount];
      advance = new float[glyphCount];

      u1 = new float[glyphCount];
      v1 = new float[glyphCount];
      u2 = new float[glyphCount];
      v2 = new float[glyphCount];
    }

    FontData::~FontData()
    {
      delete[] w;
      delete[] h;
      delete[] le;
      delete[] te;
      delete[] advance;

      delete[] u1;
      delete[] v1;
      delete[] u2;
      delete[] v2;
    }

    // ---

    FontAtlas::FontAtlas(int width, int height)
    :
    width(width),
    height(height)
    {
      data = new uint8_t[width * height](); // ZERO-FILLED
    }

    FontAtlas::~FontAtlas()
    {
      delete[] data;
    }

    void FontAtlas::addGlyph(const uint8_t *glyphData, int glyphX, int glyphY, int glyphWidth, int glyphHeight)
    {
      for (int y = 0; y < glyphHeight; y++)
      {
        for (int x = 0; x < glyphWidth; x++)
        {
          data[(y + glyphY) * width + x + glyphX] = glyphData[y * glyphWidth + x];
        }
      }
    }

    // ---

    FontTexture::FontTexture(FontAtlas *atlas, const InputSource &inputSource, bool useAnisotropy)
    :
    width(atlas->width),
    height(atlas->height),
    inputSource(inputSource)
    {
      upload(atlas, useAnisotropy);
    }

    FontTexture::~FontTexture()
    {
      discard();
    }

    void FontTexture::upload(FontAtlas *atlas, bool useAnisotropy)
    {
      assert(width == atlas->width);
      assert(height == atlas->height);

      if (!glId)
      {
        glGenTextures(1, &glId);
        glBindTexture(GL_TEXTURE_2D, glId);

        if (useAnisotropy)
        {
          float anisotropy;
          glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, atlas->data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        // ---

        LOGI << "FONT UPLOADED: " <<
        inputSource.getUri() << " | " <<
        glId << " | " <<
        width << "x" << height  <<
        endl;
      }
    }

    void FontTexture::discard()
    {
      if (glId)
      {
        LOGI << "FONT DISCARDED: " <<
        glId <<
        endl;

        // ---

        glDeleteTextures(1, &glId);
        glId = 0;
      }
    }

    bool FontTexture::reload()
    {
      if (!glId)
      {
        FontData *data;
        FontAtlas *atlas;
        tie(data, atlas) = FontManager::fetchFontDataAndAtlas(inputSource); // TODO: HANDLE FAILURE

        delete data; // WE'RE ONLY INTERESTED IN THE FontAtlas

        upload(atlas);
        delete atlas;
      }

      return bool(glId);
    }

    void FontTexture::bind()
    {
      reload();

      glBindTexture(GL_TEXTURE_2D, glId);
    }

    void FontTexture::unbind()
    {
      glBindTexture(GL_TEXTURE_2D, 0);
    }

    size_t FontTexture::getMemoryUsage() const
    {
      if (glId)
      {
        return width * height * 1.33;
      }

      return 0;
    }

    glm::vec2 FontTexture::getSize() const
    {
      return glm::vec2(width, height);
    }

    // ---

    std::shared_ptr<Font> FontManager::getFont(const InputSource &inputSource, const Font::Properties &properties)
    {
      const auto &uri = inputSource.getUri();

      auto key = make_pair(uri, properties);
      auto it1 = fonts.find(key);

      if (it1 != fonts.end())
      {
        return it1->second;
      }

      FontData *data;
      FontTexture *texture;
      auto it2 = fontDataAndTextures.find(uri);

      if (it2 == fontDataAndTextures.end())
      {
        FontAtlas *atlas;
        tie(data, atlas) = fetchFontDataAndAtlas(inputSource); // TODO: HANDLE FAILURE

        texture = new FontTexture(atlas, inputSource, properties.useAnisotropy);
        delete atlas;

        fontDataAndTextures[uri] = make_pair(unique_ptr<FontData>(data), unique_ptr<FontTexture>(texture));
      }
      else
      {
        data = it2->second.first.get();
        texture = it2->second.second.get();
      }

      auto font = shared_ptr<Font>(new Font(*this, data, texture, properties)); // make_shared WON'T WORK WITH A PROTECTED CONSTRUCTOR
      fonts[key] = font;

      return font;
    }

    FontManager::FontManager()
    :
    indices(indexBuffer->storage)
    {}

    void FontManager::shutdown()
    {
      // TODO: indexBuffer.discard()
    }

    void FontManager::unload(shared_ptr<Font> font)
    {
      for (auto it = fonts.begin(); it != fonts.end(); ++it)
      {
        if (it->second == font)
        {
          fonts.erase(it);
          break;
        }
      }

      discardUnusedTextures();
    }

    void FontManager::unload(const InputSource &inputSource)
    {
      for (auto it = fonts.begin(); it != fonts.end();)
      {
        if (it->first.first == inputSource.getUri())
        {
          it = fonts.erase(it);
        }
        else
        {
          ++it;
        }
      }

      discardUnusedTextures();
    }

    void FontManager::unload()
    {
      fonts.clear();
      discardTextures();
    }

    void FontManager::discardTextures()
    {
      for (auto &element : fontDataAndTextures)
      {
        element.second.second->discard();
      }
    }

    void FontManager::reloadTextures()
    {
      set<FontTexture*> texturesInUse;

      for (auto &font : fonts)
      {
        const auto &uri = font.first.first;
        auto it = fontDataAndTextures.find(uri);

        if (it != fontDataAndTextures.end())
        {
          texturesInUse.insert(it->second.second.get());
        }
      }

      for (auto &texture : texturesInUse)
      {
        texture->reload();
      }
    }

    size_t FontManager::getTextureMemoryUsage() const
    {
      size_t total = 0;

      for (auto &element : fontDataAndTextures)
      {
        total += element.second.second->getMemoryUsage();
      }

      return total;
    }

    void FontManager::discardUnusedTextures()
    {
      set<FontTexture*> texturesInUse;

      for (auto &font : fonts)
      {
        const auto &uri = font.first.first;
        auto it = fontDataAndTextures.find(uri);

        if (it != fontDataAndTextures.end())
        {
          texturesInUse.insert(it->second.second.get());
        }
      }

      for (auto &element : fontDataAndTextures)
      {
        if (!texturesInUse.count(element.second.second.get()))
        {
          element.second.second->discard();
        }
      }
    }

    std::pair<FontData*, FontAtlas*> FontManager::fetchFontDataAndAtlas(const InputSource &inputSource)
    {
      FontData *data = nullptr;
      FontAtlas *atlas = nullptr;

      BinaryInputStream stream(inputSource);

      if (stream.readString(9) == "XFONT.004")
      {
        stream.skip(1); // XXX

        auto glyphCount = stream.read<uint32_t>();
        data = new FontData(glyphCount);

        data->baseSize = stream.read<float>();
        data->height = stream.read<float>();
        data->ascent = stream.read<float>();
        data->descent = stream.read<float>();
        data->lineThickness = stream.read<float>();
        data->underlineOffset = stream.read<float>();
        data->strikethroughOffset = stream.read<float>();
        data->spaceAdvance = stream.read<float>();

        auto atlasWidth = stream.read<int32_t>();
        auto atlasHeight = stream.read<int32_t>();
        auto atlasPadding = stream.read<int32_t>();
        auto unitMargin = stream.read<int32_t>();
        auto unitPadding = stream.read<int32_t>();

        atlas = new FontAtlas(atlasWidth, atlasHeight);

        for (int i = 0; i < glyphCount; i++)
        {
          auto glyphChar = stream.read<uint32_t>();
          data->glyphs[(char16_t)glyphChar] = i;

          data->advance[i] = stream.read<float>();
          auto glyphWidth = stream.read<int32_t>();
          auto glyphHeight = stream.read<int32_t>();
          auto glyphLeftExtent = stream.read<int32_t>();
          auto glyphTopExtent = stream.read<int32_t>();
          auto glyphAtlasX = stream.read<int32_t>();
          auto glyphAtlasY = stream.read<int32_t>();

          auto glyphData = new uint8_t[glyphWidth * glyphHeight];
          stream.readBytes(glyphData, glyphWidth * glyphHeight);

          atlas->addGlyph(glyphData, glyphAtlasX + atlasPadding + unitMargin, glyphAtlasY + atlasPadding + unitMargin, glyphWidth, glyphHeight);
          delete[] glyphData;

          data->w[i] = glyphWidth + unitPadding * 2;
          data->h[i] = glyphHeight + unitPadding * 2;
          data->le[i] = glyphLeftExtent - unitPadding;
          data->te[i] = glyphTopExtent + unitPadding;

          int x = glyphAtlasX + atlasPadding + unitMargin - unitPadding;
          int y = glyphAtlasY + atlasPadding + unitMargin - unitPadding;

          data->u1[i] = x / (float)atlasWidth;
          data->v1[i] = y / (float)atlasHeight;
          data->u2[i] = (x + data->w[i]) / (float)atlasWidth;
          data->v2[i] = (y + data->h[i]) / (float)atlasHeight;
        }
      }

      return make_pair(data, atlas);
    }

    Buffer<GLushort>& FontManager::getIndexBuffer(size_t capacity)
    {
      if (capacity * 6 > indices.size())
      {
        /*
         * FILLING THE INDICES WITH A QUAD PATTERN:
         * - FROM UPPER-LEFT CORNER
         * - COUNTER-CLOCKWISE
         */

        indices.reserve(capacity * 6);
        indices.clear();

        size_t offset = 0;

        for (auto i = 0; i < capacity; i++)
        {
          indices.push_back(offset);
          indices.push_back(offset + 1);
          indices.push_back(offset + 2);
          indices.push_back(offset + 2);
          indices.push_back(offset + 3);
          indices.push_back(offset);

          offset += 4;
        }

        indexBuffer.requestUpload();
      }

      return indexBuffer;
    }
  }
}
