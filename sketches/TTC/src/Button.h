#pragma once

#include "EventCaster.h"

#include "chr/math/Rect.h"

#include <memory>

class UI;

class Button : public EventCaster
{
public:
  struct Style
  {
    glm::vec4 stroke, fill, body;
    glm::vec4 stroke_over, fill_over, body_over;
    glm::vec4 stroke_pressed, fill_pressed, body_pressed;
    glm::vec4 stroke_disabled, fill_disabled, body_disabled;
  };

  struct StyleShortcut
  {
    static void load(Style &style, const glm::vec4 &col1, const glm::vec4 &col2, const glm::vec4 &col3, const glm::vec4 &col4)
    {
      style.stroke = col1;
      style.fill = col2;
      style.body = col1;
      style.stroke_over = col3;
      style.fill_over = col2;
      style.body_over = col3;
      style.stroke_pressed = col3;
      style.fill_pressed = col3;
      style.body_pressed = col2;
      style.stroke_disabled = col4;
      style.fill_disabled = col2;
      style.body_disabled = col4;
    }
  };

  enum
  {
    EVENT_PRESSED
  };

  chr::math::Rectf bounds;

  Button() = default;
  Button(std::shared_ptr<UI> ui);

  void setBounds(const chr::math::Rectf &bounds);
  void reset();
  void setEnabled(bool b);
  void setLocked(bool b);

  void run();
  virtual void draw();

protected:
  std::shared_ptr<UI> ui;
  bool over, pressed = false, armed = false, disabled = false, locked = false;
  Style style;
  glm::vec4 color_stroke, color_fill, color_body;
};
