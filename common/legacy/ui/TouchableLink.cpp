#include "TouchableLink.h"

namespace chr
{
    TouchableLink::TouchableLink(Shape *shape, int linkIndex, const math::Rectf &bounds, Type type, int hitExtra)
    :
    Touchable(shape, type, hitExtra),
    linkIndex(linkIndex),
    bounds(bounds)
    {}
    
    math::Rectf TouchableLink::getTouchableBounds()
    {
        return bounds;
    }
}
