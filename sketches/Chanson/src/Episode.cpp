#include "Episode.h"
#include "Sketch.h"

#include "pugixml.hpp"

using namespace std;
using namespace chr;
using namespace pugi;

Episode::Episode(Sketch *sketch)
:
sketch(sketch),
audioManager(sketch->audioManager),
fontManager(sketch->fontManager)
{}

void Episode::performSetup()
{
  font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));

  setup();
}

void Episode::performResize(const glm::ivec2 &size)
{
  screenSize = size;
  resize(size);
}

float Episode::setupWords()
{
  float maxFactor = numeric_limits<float>::min();
  unsigned int maxWidth;

  for (vector<Word>::iterator it = words.begin(); it != words.end(); ++it)
  {
    unsigned int width = it->endOffset - it->startOffset;
    float factor = width / font->getStringAdvance(it->text);

    if (factor > maxFactor)
    {
      maxFactor = factor;
      maxWidth = width;
    }

    it->factor = factor;
  }

  for (vector<Word>::iterator it = words.begin(); it != words.end(); ++it)
  {
    it->factor /= maxFactor;
  }

  return maxFactor;
}

void Episode::loadStanza(const InputSource &source, int stanzaIndex, const Stanza::FirstIndex &firstIndex, const Stanza::LastIndex &lastIndex)
{
  auto stream = source.getStream();
  if (stream)
  {
    xml_document doc;
    if (doc.load(*stream))
    {
      auto root = doc.child("Stanza");
      auto shift = root.attribute("shift").as_int();

      // ---

      int lineIndex = 1;
      map<int, unsigned int> lineOffsets;

      for (auto lineXPathNode : root.select_nodes("Lines/Line"))
      {
        auto offset = lineXPathNode.node().attribute("offset").as_uint();
        lineOffsets[lineIndex++] = offset + shift;
      }

      // ---

      int verseIndex = 1;

      for (auto verseXPathNode : root.select_nodes("Words/Verse"))
      {
        int wordIndex = 1;

        for (auto &wordNode : verseXPathNode.node().children())
        {
          if (verseIndex >= firstIndex.verseIndex)
          {
            if ((verseIndex != firstIndex.verseIndex) || (wordIndex >= firstIndex.wordIndex))
            {
              if (verseIndex <= lastIndex.verseIndex)
              {
                if ((verseIndex != lastIndex.verseIndex) || (wordIndex <= lastIndex.wordIndex))
                {
                  auto text = utils::to<u16string>(string(wordNode.attribute("text").as_string()));
                  auto lineIndex = wordNode.attribute("lineIndex").as_int();

                  auto startOffset = lineOffsets[lineIndex];
                  auto endOffset = lineOffsets[lineIndex + 1];
                  words.emplace_back(text, startOffset, endOffset, stanzaIndex, verseIndex, wordIndex);
                }
              }
            }
          }

          wordIndex++;
        }

        verseIndex++;
      }
    }
  }
}
