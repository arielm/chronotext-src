#pragma once

#include "Shape.h"

#include <vector>
#include <memory>

namespace chr
{
  class Container : public Shape
  {
  public:
    std::vector<std::shared_ptr<Shape>> components;

    Container() = default;

    void setWidth(float newWidth) override;
    void setHeight(float newHeight) override;
    void setAutoWidth(bool autoWidth) override;
    void setAutoHeight(bool autoHeight) override;
    void setPadding(float left, float top, float right, float bottom) override;
    
    float getWidth() override;
    float getHeight() override;
    Shape* getComponentByTag(int tag);

    void addComponent(std::shared_ptr<Shape> component);
    void requestLayout();

    bool layout() override;
    void draw() override;

  protected:
    bool layoutRequest;
    
    void drawComponents();
    float mergedMargin(float previousMargin, float nextMargin);
  };
}
