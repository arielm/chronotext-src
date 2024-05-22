#pragma once

#include <string>

class Word
{
public:
    std::u16string text;
    
    unsigned int startOffset;
    unsigned int endOffset;
    
    int stanzaIndex;
    int verseIndex;
    int wordIndex;
    
    float factor;
    
    Word(const std::u16string &text, unsigned int startOffset, unsigned int endOffset, int stanzaIndex, int verseIndex, int wordIndex)
    :
    text(text),
    startOffset(startOffset),
    endOffset(endOffset),
    stanzaIndex(stanzaIndex),
    verseIndex(verseIndex),
    wordIndex(wordIndex)
    {}
};
