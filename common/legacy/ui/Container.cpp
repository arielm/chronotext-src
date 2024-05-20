#include "Container.h"

using namespace std;

namespace chr
{
  void Container::setWidth(float newWidth)
  {
    if (autoWidth || newWidth != width)
    {
      layoutRequest = true;
      Shape::setWidth(newWidth);
    }
  }
  
  void Container::setHeight(float newHeight)
  {
    if (autoHeight || newHeight != height)
    {
      layoutRequest = true;
      Shape::setHeight(newHeight);
    }
  }
  
  void Container::setAutoWidth(bool autoWidth)
  {
    if (autoWidth != this->autoWidth)
    {
      layoutRequest = true;
      Shape::setAutoWidth(autoWidth);
    }
  }
  
  void Container::setAutoHeight(bool autoHeight)
  {
    if (autoHeight != this->autoHeight)
    {
      layoutRequest = true;
      Shape::setAutoHeight(autoHeight);
    }
  }
  
  void Container::setPadding(float left, float top, float right, float bottom)
  {
    layoutRequest = true;
    Shape::setPadding(left, top, right, bottom);
  }
  
  float Container::getWidth()
  {
    layout();
    return width;
  }
  
  float Container::getHeight()
  {
    layout();
    return height;
  }
  
  Shape* Container::getComponentByTag(int tag)
  {
    for (auto &component : components)
    {
      if (component->tag == tag)
      {
        return component.get();
      }
    }
    
    return nullptr;
  }
  
  void Container::addComponent(shared_ptr<Shape> component)
  {
    layoutRequest = true;
    components.push_back(component);
    component->setContainer(this);
  }
  
  void Container::requestLayout()
  {
    layoutRequest = true;
  }

  bool Container::layout()
  {
    bool result = layoutRequest;

    if (layoutRequest)
    {
      layoutRequest = false;

      float innerWidth = width - paddingLeft - paddingRight;
      float innerHeight = 0;
      float previousMargin = paddingTop;
      float limitLeft = x + paddingLeft;

      for (auto component : components)
      {
        innerHeight += fmaxf(previousMargin, component->marginTop);

        component->setLocation(limitLeft, y + innerHeight);
        component->setWidth(innerWidth);
        component->layout();

        innerHeight += component->getHeight();
        previousMargin = component->marginBottom;
      }

      height = innerHeight + fmaxf(previousMargin, paddingBottom);
    }

    return result;
  }

  void Container::draw()
  {
    layout();
    drawComponents();
  }
  
  void Container::drawComponents()
  {
    for (auto &component : components)
    {
      if (component->visible)
      {
        component->draw();
      }
    }
  }
  
  float Container::mergedMargin(float previousMargin, float nextMargin)
  {
    if ((previousMargin > 0) && (nextMargin > 0))
    {
      return max<float>(previousMargin, nextMargin);
    }
    else if ((previousMargin < 0) && (nextMargin < 0))
    {
      return min<float>(previousMargin, nextMargin);
    }
    
    return previousMargin + nextMargin;
  }
}
