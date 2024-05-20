#include "ClockItem.h"
#include "UI.h"

#include "chr/utils/Utils.h"
#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

ClockItem::ClockItem(shared_ptr<UI> ui)
:
ui(ui)
{}

float ClockItem::getWidth()
{
  return ui->subFont->getStringAdvance(u"00:00:00") + ui->gap * ui->scale * 2 * 2;
}

void ClockItem::setBounds(const Rectf &bounds)
{
  this->bounds = bounds;
}

void ClockItem::setMediaTime(int time)
{
  this->time = time;
}

void ClockItem::draw()
{
  int t = time /= 1000;
  int hours = t / 3600;
  int minutes = (t / 60) - (hours * 60);
  int seconds = t - (hours * 3600) - (minutes * 60);

  string s = ((hours < 10 ? "0" : "") + utils::toString(hours)
    + ":" + (minutes < 10 ? "0" : "") + utils::toString(minutes)
    + ":" + (seconds < 10 ? "0" : "") + utils::toString(seconds));

  // ---

  draw::Rect()
    .setColor(0x33 / 255.0f, 0x33 / 255.0f, 0x33 / 255.0f, 1)
    .setBounds(bounds)
    .append(ui->flatBatch);

  ui->subFont->setColor(1, 1, 1, 1);
  ui->drawTextInRect(utils::to<u16string>(s), bounds, ui->subFontShift * ui->scale);
}
