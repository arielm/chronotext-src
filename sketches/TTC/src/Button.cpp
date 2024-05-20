#include "Button.h"
#include "UI.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace math;
using namespace gl;

Button::Button(shared_ptr<UI> ui)
:
ui(ui),
style(ui->defaultStyle)
{}

void Button::setBounds(const Rectf &bounds)
{
  this->bounds = bounds;
}

void Button::reset()
{
  over = pressed = armed = disabled = locked = false;
}

void Button::setEnabled(bool b)
{
  disabled = !b;
}

void Button::setLocked(bool b)
{
  locked = b;
}

void Button::run()
{
  over = !disabled && !armed && bounds.contains(ui->mouse.position);
  armed = !over && ui->mouse.pressed;

  if (!locked && !pressed && over && ui->mouse.pressed)
  {
    castEvent(EVENT_PRESSED);
    pressed = true;
  }

  pressed = !disabled && pressed && ui->mouse.pressed;
}

void Button::draw()
{
  color_stroke = disabled ? style.stroke_disabled : ((pressed || locked) ? style.stroke_pressed : (over ? style.stroke_over : style.stroke));
  color_fill = disabled ? style.fill_disabled : ((pressed || locked) ? style.fill_pressed : (over ? style.fill_over : style.fill));
  color_body = disabled ? style.body_disabled : ((pressed || locked) ? style.body_pressed : (over ? style.body_over : style.body));

  draw::Rect()
    .setColor(color_fill)
    .setBounds(bounds)
    .append(ui->flatBatch);

  ui->drawFrame(bounds, color_stroke);
}
