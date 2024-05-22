#pragma once

#include "Word.h"

#include "chr/glm.h"

#include <vector>

struct Brick2
{
  unsigned int startOffset;
  unsigned int endOffset;

  int stanzaIndex;
  int verseIndex;
  int wordIndex;

  float slotWidth;
  float textWidth;

  std::u16string text;
  bool hasText;

  float position;
  bool visible;

  float marginLeft;
  float marginRight;
  float v;

  glm::vec3 anchor;
  float ay;

  Brick2(unsigned int startOffset, unsigned int endOffset, float slotWidth)
  :
  startOffset(startOffset),
  endOffset(endOffset),
  slotWidth(slotWidth),
  hasText(false),
  marginLeft(0),
  marginRight(0),
  v(0)
  {}

  Brick2(float slotWidth, float textWidth, const std::vector<Word>::const_iterator &word)
  :
  slotWidth(slotWidth),
  textWidth(textWidth),
  startOffset(word->startOffset),
  endOffset(word->endOffset),
  stanzaIndex(word->stanzaIndex),
  verseIndex(word->verseIndex),
  wordIndex(word->wordIndex),
  text(word->text),
  hasText(true),
  marginLeft(0),
  marginRight(0),
  v(0)
  {}
};
