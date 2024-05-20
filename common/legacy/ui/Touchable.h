#pragma once

#include "chr/math/Rect.h"

namespace chr
{
    class Shape;

    class Touchable
    {
    public:
        enum State
        {
            STATE_NORMAL,
            STATE_PRESSED,
            STATE_TOGGLED,
            STATE_DISABLED,
        };
        
        enum Type
        {
            TYPE_CLICKABLE,
            TYPE_PRESSABLE,
            TYPE_TOGGLABLE,
        };
        
        enum Action
        {
            ACTION_CLICKED,
            ACTION_PRESSED,
            ACTION_TOGGLED,
        };
        
        Shape *shape;
        Touchable::State state = STATE_NORMAL;
        Touchable::Type type;
        int armedIndex = -1;
        float hitExtra;
        
        Touchable(Shape *shape, Touchable::Type type, int hitExtra = 0);
        virtual ~Touchable() {}

        virtual math::Rectf getTouchableBounds() { return math::Rectf(0, 0, 0, 0); } // ONLY RELEVANT FOR RECTANGULAR-TOUCHABLES
        virtual bool hitTest(const glm::vec2 &point, float *distance); // USE THE DEFAULT (I.E. RECTANGULAR) getTouchableBounds()
        
        void changeState(State nextState);
        void performAction(Action action);
    };
}
