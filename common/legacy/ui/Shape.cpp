#include "Shape.h"
#include "Container.h"

using namespace std;
using namespace chr;
using namespace math;

namespace chr
{
  void Shape::setLocation(float x, float y)
  {
    this->x = x;
    this->y = y;
  }
  
  void Shape::setWidth(float width)
  {
    this->width = width;
    autoWidth = false;
    requestContainerLayout();
  }
  
  void Shape::setHeight(float height)
  {
    this->height = height;
    autoHeight = false;
    requestContainerLayout();
  }
  
  void Shape::setAutoWidth(bool autoWidth)
  {
    this->autoWidth = autoWidth;
    requestContainerLayout();
  }
  
  void Shape::setAutoHeight(bool autoHeight)
  {
    this->autoHeight = autoHeight;
    requestContainerLayout();
  }
  
  void Shape::setBounds(const Rectf &bounds)
  {
    setLocation(bounds.x1, bounds.y1);
    setWidth(bounds.width());
    setHeight(bounds.height());
  }
  
  void Shape::setPadding(float left, float top, float right, float bottom)
  {
    paddingLeft = left;
    paddingTop = top;
    paddingRight = right;
    paddingBottom = bottom;
  }
  
  void Shape::setMargin(float left, float top, float right, float bottom)
  {
    marginLeft = left;
    marginTop = top;
    marginRight = right;
    marginBottom = bottom;
    
    requestContainerLayout();
  }
  
  void Shape::setVisible(bool visible)
  {
    this->visible = visible;
    requestContainerLayout();
  }

  glm::vec2 Shape::getLocation()
  {
    return glm::vec2(x, y);
  }
  
  float Shape::getWidth()
  {
    return width;
  }
  
  float Shape::getHeight()
  {
    return height;
  }
  
  Rectf Shape::getBounds()
  {
    return Rectf(x, y, getWidth(), getHeight());
  }

  void Shape::setContainer(Container *container)
  {
    this->container = container;
  }
  
  void Shape::requestContainerLayout()
  {
    if (container)
    {
      container->requestLayout();
    }
  }
}
