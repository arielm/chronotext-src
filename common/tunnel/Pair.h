#pragma once

#include "common/tunnel/ItemList.h"
#include "common/tunnel/Target.h"

class Pair
{
public:
    Item item;
    Target target;
    float delta;
    
    Pair(const Item &item, const Target &target) : item(item), target(target), delta(0) {}
};
