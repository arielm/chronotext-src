#pragma once

class Target
{
public:
    int stringId;
    int index;
    
    Target() {}
    Target(int stringId, int index) : stringId(stringId), index(index) {}
};
