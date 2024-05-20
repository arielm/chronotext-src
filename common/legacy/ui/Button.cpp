#include "common/legacy/ui/Button.h"

#include "chr/gl/draw/Rect.h"
#include "chr/gl/draw/Sprite.h"

using namespace chr;
using namespace gl;
using namespace math;

Button& Button::setId(int id)
{
  this->id = id;
  return *this;
}

Button& Button::setType(Type type)
{
  this->type = type;
  return *this;
}

Button& Button::setBounds(const Rectf &bounds)
{
  this->bounds = bounds;
  return *this;
}

Button& Button::setTextureBounds(const chr::math::Rectf &bounds)
{
  textureBounds = bounds;
  return *this;
}

Button& Button::setHitExtra(float hitExtra)
{
  this->hitExtra = hitExtra;
  return *this;
}

Button& Button::setStyle(const Style &style)
{
  this->style = style;
  return *this;
}

Rectf Button::getBounds() const
{
  return bounds;
}

glm::vec2 Button::getSize() const
{
  return textureBounds.size();
}

bool Button::hitTest(const glm::vec2 &point, float *distance)
{
  Rectf extra = bounds.inflated(glm::vec2(hitExtra));

  if (extra.contains(point))
  {
    *distance = bounds.distance(point);
    return true;
  }

  return false;
}

void Button::draw(IndexedVertexBatch<XYZ.RGBA> &fillBatch, IndexedVertexBatch<XYZ.UV.RGBA> &textureBatch)
{
  draw::Rect()
    .setBounds(bounds)
    .setColor(style.backgroundColors[state])
    .append(fillBatch);

  draw::Sprite()
    .setAnchor(0.5f, 0.5f)
    .setTextureBounds(textureBounds)
    .setColor(style.colors[state])
    .append(textureBatch, bounds.center());
}

void Button::changeState(State nextState)
{
  if (state != nextState)
  {
    state = nextState;
  }
}

