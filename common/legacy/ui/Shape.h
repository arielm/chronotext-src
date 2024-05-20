#pragma once

#include "Touchable.h"

#include <vector>

namespace chr
{
  class Container;

  class Shape
  {
  public:
    enum
    {
      ALIGN_LEFT,
      ALIGN_RIGHT,
      ALIGN_TOP,
      ALIGN_BOTTOM,
      ALIGN_MIDDLE
    };

    float x = 0;
    float y = 0;

    float width = 0;
    float height = 0;
    
    bool autoWidth = true;
    bool autoHeight = true;
    
    float paddingLeft = 0;
    float paddingTop = 0;
    float paddingRight = 0;
    float paddingBottom = 0;

    float marginLeft = 0;
    float marginTop = 0;
    float marginRight = 0;
    float marginBottom = 0;

    bool visible = true;
    int tag = 0;
    Container *container = nullptr;
    
    Shape() = default;
    ~Shape() {}

    virtual void setLocation(float x, float y);
    virtual void setWidth(float width);
    virtual void setHeight(float height);
    virtual void setAutoWidth(bool autoWidth);
    virtual void setAutoHeight(bool autoHeight);
    virtual void setBounds(const math::Rectf &bounds);
    virtual void setPadding(float left, float top, float right, float bottom);
    virtual void setMargin(float left, float top, float right, float bottom);
    virtual void setVisible(bool visible);
    
    virtual glm::vec2 getLocation();
    virtual float getWidth();
    virtual float getHeight();
    virtual math::Rectf getBounds();
    
    virtual void draw() {};
    
    virtual void setContainer(Container *container);
    virtual void requestContainerLayout();
    virtual bool layout() { return false; }

    virtual std::vector<Touchable*> getTouchables() { return std::vector<Touchable*>(); }
    virtual void touchStateChanged(Touchable *touchable, Touchable::State nextState, Touchable::State prevState) {}
    virtual void touchActionPerformed(Touchable *touchable, Touchable::Action action) {}
  };
}
