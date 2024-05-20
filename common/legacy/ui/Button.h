#pragma once

#include "chr/gl/Batch.h"

class Button
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

  struct Style
  {
    std::map<State, glm::vec4> colors;
    std::map<State, glm::vec4> backgroundColors;
  };

  class Handler
  {
  public:
    ~Handler() {}
    virtual void buttonEvent(int id, Action action) = 0;
  };

  int id = 0;
  Type type = TYPE_CLICKABLE;
  State state = STATE_NORMAL;
  int armedIndex = -1;

  Button& setId(int id);
  Button& setType(Type type);
  Button& setBounds(const chr::math::Rectf &bounds);
  Button& setTextureBounds(const chr::math::Rectf &bounds);
  Button& setHitExtra(float hitExtra);
  Button& setStyle(const Style &style);

  chr::math::Rectf getBounds() const;
  glm::vec2 getSize() const;

  bool hitTest(const glm::vec2 &point, float *distance);
  void draw(chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> &fillBatch, chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV.RGBA> &textureBatch);

  void changeState(State nextState);
  void performAction(Action action);

protected:
  chr::math::Rectf bounds;
  chr::math::Rectf textureBounds;
  float hitExtra = 0;
  Style style;
};
