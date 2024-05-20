#include "Lines.h"

using namespace std;
using namespace chr;

void Lines::setText(shared_ptr<XFont> font, const u16string &text)
{
  this->font = font;
  this->text = text;
}

void Lines::wrap(float width)
{
  indices.clear();
  lengths.clear();

  size = 0;
  int length = text.size();

  int spaceIndex = -2; // lastToken = 2
  int wordIndex = -2; // lastToken = 3
  int hyphenIndex = -2;
  int brokenIndex = -2;
  int lastToken = 0;
  bool newLine = true;
  int lineIndex = 0;
  int lineLength = 0;
  char16_t c = 0;
  float x = 0;

  for (int i = 0; i < length; i++)
  {
    char16_t lastC = c;
    c = text[i];
    char16_t nextC = (i + 1) < length ? text[i + 1] : 0;

    if (c == '\r' || c == '\n') // win, unix, osx
    {
      if (lastC == '\r' && c == '\n') // win
      {
        lineIndex = i + 1;
      }
      else
      {
        if (brokenIndex != i - 1)
        {
          addLine(lineIndex, lineLength);
        }

        lineIndex = i + 1;
        lineLength = 0;
        x = 0;
        lastToken = 0;
        newLine = true;
      }
      continue;
    }

    if (c == ' ' && (lastC != ' ' || lineIndex == i))
    {
      spaceIndex = i;
      lastToken = 2;
    }
    else if (c != ' ' && (lastC == ' ' || lineIndex == i || hyphenIndex == i - 1))
    {
      wordIndex = i;
      lastToken = 3;
    }
    else if (wordIndex != i && c == '-' && lastC != '-' && nextC != 0 && nextC != ' ' && nextC != '\r' && nextC != '\n')
    {
      hyphenIndex = i;
    }

    float charWidth = font->getCharAdvance(c);

    if (x + charWidth >= width)
    {
      if (lastToken == 2) // lines ending with spaces
      {
        while (spaceIndex + 1 < length && text[spaceIndex + 1] == ' ')
        {
          spaceIndex++;
        }
        if (newLine || wordIndex >= lineIndex)
        {
          addLine(lineIndex, spaceIndex - lineIndex);
        }

        lineIndex = spaceIndex + 1;
        i = brokenIndex = lineIndex - 1;
      }
      else if (lastToken == 3 && hyphenIndex >= lineIndex && hyphenIndex < wordIndex) // hyphen-break
      {
        addLine(lineIndex, wordIndex - lineIndex);
        lineIndex = wordIndex;
        i = lineIndex - 1;
      }
      else if (lastToken == 3 && spaceIndex >= lineIndex) // word-break
      {
        addLine(lineIndex, wordIndex - lineIndex);
        lineIndex = wordIndex;
        i = lineIndex - 1;
      }
      else // long-line break
      {
        lineLength += lineLength == 0 ? 1 : 0; // for extremely narrowed-width cases
        addLine(lineIndex, lineLength);
        lineIndex = lineIndex + lineLength;
        i = brokenIndex = lineIndex - 1;
      }

      c = text[i];
      lineLength = 0;
      x = 0;
      lastToken = 0;
      newLine = false;
    }
    else if (x == 0 && c == ' ') // spaces at the beginning of lines (new lines or not) are never enabled
    {
      lineIndex++;
    }
    else
    {
      lineLength++;
      x += charWidth;
    }
  }

  addLine(lineIndex, lineLength);
}

float Lines::getLineWidth(int index)
{
  return font->getStringAdvance(text.substr(indices[index], lengths[index]));
}

void Lines::addLine(int index, int length)
{
  if (length > 0)
  {
    while (length > 1 && text[index + length - 1] == ' ')
    {
      length--;
    }

    if (length > 0)
    {
      indices.push_back(index);
      lengths.push_back(length);
      size++;
    }
  }
}
