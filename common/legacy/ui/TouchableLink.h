#pragma once

#include "Touchable.h"

namespace chr
{
    class TouchableLink : public Touchable
    {
    public:
        math::Rectf bounds;
        int linkIndex;
        
        TouchableLink(Shape *shape, int linkIndex, const math::Rectf &bounds, Type type, int hitExtra);
        
        math::Rectf getTouchableBounds();
    };
}
