#include "TextItem.h"
#include "UI.h"

using namespace std;
using namespace chr;

TextItem::TextItem(shared_ptr<UI> ui, const u16string &text, XFont::Alignment alignX)
:
ui(ui),
text(text),
alignX(alignX)
{}

float TextItem::getWidth()
{
  return ui->subFont->getStringAdvance(text);
}

void TextItem::setPosition(const glm::vec2 &position)
{
  this->position = position;
}

void TextItem::draw()
{
  ui->subFont->setColor(1, 1, 1, 1);
  ui->drawText(text, position, alignX);
}
