#pragma once

#include "Shape.h"

namespace chr
{
    class TouchGroup
    {
    public:
        TouchGroup() = default;
        
        void addShape(Shape *shape);
        
        void pressTouchable(Touchable *touchable);
        void toggleTouchable(Touchable *touchable);
        void clickTouchable(Touchable *touchable);
        
        bool addTouch(int index, const glm::vec2 &point);
        bool updateTouch(int index, const glm::vec2 &point);
        bool removeTouch(int index, const glm::vec2 &point);
        
    protected:
        Touchable* toggledTouchable = nullptr;
        bool toggledOrDisabledIsArmed = false;
        
        std::vector<Shape*> shapes;
        
        Touchable* getClosestTouchable(const glm::vec2 &point);
        Touchable* getArmedTouchableByIndex(int index);
    };
}
