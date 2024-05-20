#include "AboutScreen.h"
#include "UI.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace math;

AboutScreen::AboutScreen(shared_ptr<UI> ui)
:
Screen(ui)
{
  itemOptions = TextItem(ui, u"options", XFont::ALIGN_RIGHT);
  buttonOptions = ImageButton(ui, "options");

  //

  auto text = utils::readText<u16string>(InputSource::resource("about.txt"));

  textBox.setText(text);
  textBox.setAutoHeight(true);
  textBox.setWrap(true);
  textBox.setFont(ui->subFont);
  textBox.setLineHeightFactor(1.2f);
  textBox.setTextColor(glm::vec4(1, 1, 1, 1)); // XXX
}

void AboutScreen::resize()
{
  float buttonDimension = ui->deckH * ui->scale;
  float gap = ui->gap * ui->scale;
  float padding = 16 * ui->scale;
  float x = (ui->windowSize.x - ui->windowSize.y) * 0.5f;
  float y = ui->windowSize.y - padding - buttonDimension;

  float x5 = x + ui->windowSize.y - padding - buttonDimension;
  float x6 = x5 - gap;

  //

  itemOptions.setPosition(glm::vec2(x6, y + buttonDimension * 0.5f));
  buttonOptions.setBounds(Rectf(x5, y, buttonDimension, buttonDimension));

  //

  textBox.setFontSize(ui->subFontSize * ui->scale);
  textBox.setWidth(400 * ui->scale - padding * 2);
  textBox.setLocation(x + padding, padding);
}

void AboutScreen::run()
{
  buttonOptions.run();
}

void AboutScreen::draw()
{
  ui->beginGroupDraw();
  itemOptions.draw();
  buttonOptions.draw();
  ui->endGroupDraw();

  //

  textBox.draw();
}

void AboutScreen::enter()
{
  buttonOptions.setListener(this);
  buttonOptions.reset();
}

void AboutScreen::event(EventCaster *source, int message)
{
  if (source == &buttonOptions)
  {
    ui->setScreen(ui->mainScreen);
  }
}
