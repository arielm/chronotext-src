#pragma once

#include <limits>

class Stanza
{
public:
    struct FirstIndex
    {
        int verseIndex;
        int wordIndex;
        
        FirstIndex(int verseIndex = 1, int wordIndex = 1)
        :
        verseIndex(verseIndex),
        wordIndex(wordIndex)
        {}
    };
    
    struct LastIndex
    {
        int verseIndex;
        int wordIndex;
        
        LastIndex(int verseIndex = std::numeric_limits<int>::max(), int wordIndex = std::numeric_limits<int>::max())
        :
        verseIndex(verseIndex),
        wordIndex(wordIndex)
        {}
    };
};
