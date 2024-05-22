/*
 * RELATIVELY SIMILAR TO Brick3
 */

#pragma once

#include "Word.h"

#include "chr/path/FollowablePath3D.h"

struct Brick1
{
    unsigned int startOffset;
    unsigned int endOffset;
    
    int stanzaIndex;
    int verseIndex;
    int wordIndex;
    
    float position;
    float width;
    float factor;
    
    std::u16string text;
    bool hasText;
    
    int fullSegmentCount;
    int partialSegmentCount;
    float start;
    float end;

    chr::path::FollowablePath3D path;

    Brick1(unsigned int startOffset, unsigned int endOffset, float position, float width)
    :
    startOffset(startOffset),
    endOffset(endOffset),
    position(position),
    width(width),
    factor(1),
    hasText(false),
    start(0),
    end(1)
    {}
    
    Brick1(float position, float width, float factor, const std::vector<Word>::const_iterator &word)
    :
    position(position),
    width(width),
    factor(factor),
    startOffset(word->startOffset),
    endOffset(word->endOffset),
    stanzaIndex(word->stanzaIndex),
    verseIndex(word->verseIndex),
    wordIndex(word->wordIndex),
    text(word->text),
    hasText(true),
    start(0),
    end(1)
    {}
};
