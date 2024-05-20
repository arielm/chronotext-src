#include "TouchGroup.h"

namespace chr
{
    void TouchGroup::addShape(Shape *shape)
    {
        shapes.push_back(shape);
    }
    
    void TouchGroup::pressTouchable(Touchable *touchable)
    {
        touchable->performAction(Touchable::ACTION_PRESSED);
    }

    void TouchGroup::toggleTouchable(Touchable *touchable)
    {
        if (toggledTouchable)
        {
            toggledTouchable->changeState(Touchable::STATE_NORMAL);
        }
        
        toggledTouchable = touchable;
        
        touchable->changeState(Touchable::STATE_TOGGLED);
        touchable->performAction(Touchable::ACTION_TOGGLED);
    }
    
    void TouchGroup::clickTouchable(Touchable *touchable)
    {
        touchable->changeState(Touchable::STATE_NORMAL);
        touchable->performAction(Touchable::ACTION_CLICKED);
    }
    
    bool TouchGroup::addTouch(int index, const glm::vec2 &point)
    {
        Touchable* closestTouchable = getClosestTouchable(point);
        
        if (closestTouchable)
        {
            if (closestTouchable->state == Touchable::STATE_DISABLED)
            {
                toggledOrDisabledIsArmed = true;
                return true;
            }
            else if (closestTouchable->type == Touchable::TYPE_TOGGLABLE)
            {
                if (closestTouchable->state == Touchable::STATE_NORMAL)
                {
                    toggleTouchable(closestTouchable);
                }
                
                toggledOrDisabledIsArmed = true;
                return true;
            }
            else if (closestTouchable->type == Touchable::TYPE_PRESSABLE)
            {
                pressTouchable(closestTouchable);
                return true;
            }
            else if (closestTouchable->type == Touchable::TYPE_CLICKABLE)
            {
                if (closestTouchable->armedIndex == -1)
                {
                    closestTouchable->armedIndex = index;
                    closestTouchable->changeState(Touchable::STATE_PRESSED);
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool TouchGroup::updateTouch(int index, const glm::vec2 &point)
    {
        Touchable *armedTouchable = getArmedTouchableByIndex(index);
        
        if (armedTouchable)
        {
            if (armedTouchable == getClosestTouchable(point))
            {
                armedTouchable->changeState(Touchable::STATE_PRESSED);
                return true;
            }
            else
            {
                armedTouchable->changeState(Touchable::STATE_NORMAL);
                return true;
            }
        }
        
        return toggledOrDisabledIsArmed;
    }
    
    bool TouchGroup::removeTouch(int index, const glm::vec2 &point)
    {
        Touchable *armedTouchable = getArmedTouchableByIndex(index);
        
        if (armedTouchable)
        {
            armedTouchable->armedIndex = -1;
            
            if (armedTouchable->state == Touchable::STATE_PRESSED)
            {
                clickTouchable(armedTouchable);
                return true;
            }
        }
        
        if (toggledOrDisabledIsArmed)
        {
            toggledOrDisabledIsArmed = false;
            return true;
        }
        
        return false;
    }
    
    Touchable* TouchGroup::getClosestTouchable(const glm::vec2 &point)
    {
        Touchable *closestTouchable = nullptr;
        float closestDistance = FLT_MAX;
        
        for (auto &shape : shapes)
        {
            if (shape->visible)
            {
                for (auto &touchable : shape->getTouchables())
                {
                    float distance;
                    if (touchable->hitTest(point, &distance))
                    {
                        if (distance < closestDistance)
                        {
                            closestDistance = distance;
                            closestTouchable = touchable;
                        }
                    }
                }
            }
        }
        
        return closestTouchable;
    }
    
    Touchable* TouchGroup::getArmedTouchableByIndex(int index)
    {
        for (auto &shape : shapes)
        {
            for (auto &touchable : shape->getTouchables())
            {
                if (touchable->armedIndex == index)
                {
                    return touchable;
                }
            }
        }
        
        return nullptr;
    }
}
