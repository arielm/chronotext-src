#include "Touchable.h"
#include "Shape.h"

namespace chr
{
    Touchable::Touchable(Shape *shape, Touchable::Type type, int hitExtra)
    :
    shape(shape),
    type(type),
    hitExtra(hitExtra)
    {}
    
    bool Touchable::hitTest(const glm::vec2 &point, float *distance)
    {
        math::Rectf extra = getTouchableBounds().inflated(glm::vec2(hitExtra, hitExtra));
        
        if (extra.contains(point))
        {
            *distance = getTouchableBounds().distance(point);
            return true;
        }
        
        return false;
    }
    
    void Touchable::changeState(Touchable::State nextState)
    {
        if (state != nextState)
        {
            shape->touchStateChanged(this, nextState, state);
            state = nextState;
        }
    }
    
    void Touchable::performAction(Action action)
    {
        shape->touchActionPerformed(this, action);
    }
}
