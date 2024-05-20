#include "TextButton.h"

#include "UI.h"

using namespace std;
using namespace chr;
using namespace math;

TextButton::TextButton(shared_ptr<UI> ui, const u16string &text)
:
Button(ui),
text(text)
{}

void TextButton::draw()
{
  Button::draw();

  ui->subFont->setColor(color_body);
  ui->drawTextInRect(text, bounds, ui->subFontShift * ui->scale);
}
