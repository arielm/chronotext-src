#include "WarTextBox.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;

void WarTextBox::setText(const u16string &text)
{
  TextBox::setText(text);
  spans.resize(text.size());
}

int WarTextBox::updateRangeIds(int id, int start, int end, int hitsPerChar)
{
  for (int i = start; i < end; i++)
  {
    if (spans[i])
    {
      return 0;
    }
  }

  int numSpans = 0;

  auto firstLine = utils::search(wrapper.offsets, start, 1, wrapper.size + 1); // XXX
  auto lastLine = utils::search(wrapper.offsets, end, 1, wrapper.size + 1); // XXX

  for (auto i = firstLine; i <= lastLine; i++)
  {
    int lineStart = wrapper.offsets[i];
    int lineEnd = lineStart + wrapper.lengths[i];

    int spanStart = max(start, lineStart);
    int spanEnd = min(end, lineEnd);

    if (spanStart != spanEnd)
    {
      auto span = make_shared<TextSpan>(*this, id, spanStart, spanEnd, hitsPerChar);
      numSpans++;

      for (int j = spanStart; j < spanEnd; j++)
      {
        spans[j] = span;
      }
    }
  }

  return numSpans;
}

vector<shared_ptr<TextSpan>> WarTextBox::updateSpans()
{
  layout();

  // --- PASS 1 ---

  float yy = y + paddingTop;

  for (auto i = 0; i < wrapper.size; i++)
  {
    float xx = x + paddingLeft;

    float x0 = xx;
    shared_ptr<TextSpan> span;

    int start = wrapper.offsets[i];
    int end = start + wrapper.lengths[i];

    while (start < end)
    {
      if (!span && spans[start])
      {
        span = spans[start];
        x0 = xx;
      }
      else if (spans[start] != span)
      {
        span->update(i, x0, yy, xx, yy + lineHeight);
        list.push_back(span);
        span.reset();
      }

      xx += font->getCharAdvance(text[start++]);
    }

    if (span)
    {
      span->update(i, x0, yy, xx, yy + lineHeight);
      list.push_back(span);
      span.reset();
    }

    yy += lineHeight;
  }

  // --- PASS 2 ---

  for (auto span : list)
  {
    vector<shared_ptr<TextSpan>> topSpans;
    vector<shared_ptr<TextSpan>> bottomSpans;

    if (span->line - 1 >= 0)
    {
      topSpans = getRangeSpans(span->line - 1, span->x1, span->x2);
    }

    if (span->line + 1 < wrapper.size)
    {
      bottomSpans = getRangeSpans(span->line + 1, span->x1, span->x2);
    }

    span->update(topSpans, bottomSpans);
  }

  return list;
}

vector<shared_ptr<TextSpan>> WarTextBox::getRangeSpans(int line, float x1, float x2)
{
  auto start = wrapper.offsets[line];
  auto end = start + wrapper.lengths[line];

  float o1 = x + paddingLeft;
  auto start1 = start;

  while (start1 < end)
  {
    o1 += font->getCharAdvance(text[start1]);

    if (o1 >= x1)
    {
      break;
    }

    start1++;
  }
  int startIndex = start1;

  o1 = x + paddingLeft;
  start1 = start;

  while (start1 < end)
  {
    o1 += font->getCharAdvance(text[start1]);
    start1++;

    if (o1 >= x2)
    {
      break;
    }
  }
  int endIndex = start1;

  // ---

  int count = 0;
  shared_ptr<TextSpan> last;

  for (int i = startIndex; i < endIndex; i++)
  {
    if (spans[i] != last)
    {
      count++;
    }
  }

  vector<shared_ptr<TextSpan>> rangeSpans;
  rangeSpans.resize(endIndex - startIndex);

  if (count > 0)
  {
    count = 0;
    last.reset();

    for (auto i = startIndex; i < endIndex; i++)
    {
      if (spans[i] != last)
      {
        rangeSpans[count++] = spans[i];
      }
    }
  }

  return rangeSpans; // XXX
}

void WarTextBox::drawText(DrawingContext &context)
{
  draw();

  font->setSize(fontSize);
  font->setColor(textColor);
  font->beginSequence(spanSequence);

  for (auto span : list)
  {
    span->draw(context, 2);
    span->draw(context, 3);
  }

  font->endSequence();
  font->replaySequence(spanSequence);
}

void WarTextBox::drawWalls(DrawingContext &context)
{
  for (auto span : list)
  {
    span->draw(context, 0);
    span->draw(context, 1);
  }
}

void WarTextBox::drawTextures(DrawingContext &context)
{
  for (auto span : list)
  {
    span->draw(context, 4);
  }
}

void WarTextBox::drawOutlines(DrawingContext &context)
{
  for (auto span : list)
  {
    span->draw(context, 5);
  }
}

void WarTextBox::updateBatch()
{
  float yy = y + paddingTop + font->getAscent();

  font->setColor(textColor);
  font->beginSequence(sequence);

  for (int i = 0; i < wrapper.size; i++)
  {
    float xx = x + paddingLeft;
    int start = wrapper.offsets[i];
    int end = start + wrapper.lengths[i];

    while (start < end)
    {
      int glyphIndex = font->getGlyphIndex(text[start]);

      if (!spans[start++])
      {
        font->addGlyph(glyphIndex, xx, yy);
      }

      xx += font->getGlyphAdvance(glyphIndex);;
    }

    yy += lineHeight;
  }

  font->endSequence();
}
