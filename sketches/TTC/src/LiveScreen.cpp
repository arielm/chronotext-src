#include "LiveScreen.h"
#include "UI.h"

using namespace std;
using namespace chr;
using namespace math;

LiveScreen::LiveScreen(shared_ptr<UI> ui)
:
Screen(ui)
{
  buttonRecord = ImageButton(ui, "record");
  buttonStop = ImageButton(ui, "stop");

  itemClock = ClockItem(ui);
}

void LiveScreen::setCurrentTree(Tree *tree)
{
  currentTree = tree;
}

void LiveScreen::resize()
{
  ui->sharedGroup->resize();

  //

  float gap = ui->gap * ui->scale;
  float height = ui->deckH * ui->scale;
  float width = ui->windowSize.y;
  float x = (ui->windowSize.x - ui->windowSize.y) * 0.5f;
  float y = ui->deckY * ui->scale;

  float w1 = itemClock.getWidth();
  float x7 = x + (width - w1) * 0.5f;

  float x13 = x7 - gap - height;
  float x14 = x13 - gap - height;

  buttonRecord.setBounds(Rectf(x14, y, height, height));
  buttonStop.setBounds(Rectf(x13, y, height, height));

  itemClock.setBounds(Rectf(x7, y, w1, height));
}

void LiveScreen::run()
{
  currentTree->run();

  ui->sharedGroup->run(currentTree);

  //

  itemClock.setMediaTime(currentTree->stream->getCurrentMediaTime());

  buttonRecord.run();
  buttonStop.run();
}

void LiveScreen::draw()
{
  currentTree->draw();

  ui->beginGroupDraw();
  ui->sharedGroup->draw();
  itemClock.draw();
  buttonRecord.draw();
  buttonStop.draw();
  ui->endGroupDraw();
}

void LiveScreen::enter()
{
  ui->sharedGroup->enter(this, currentTree);

  //

  buttonRecord.setListener(this);
  buttonRecord.reset();

  buttonStop.setListener(this);
  buttonStop.reset();
  buttonStop.setEnabled(false);

  // starting to record automatically...
  buttonRecord.setLocked(true);
  buttonStop.setEnabled(true);
  currentTree->start(Stream::MODE_LIVE, 0); // TODO: USE MILLIS SINCE EPOCH
}

void LiveScreen::event(EventCaster *source, int message)
{
  ui->sharedGroup->event(currentTree, source, message);

  //

  if (source == &buttonRecord)
  {
    buttonRecord.setLocked(true);
    buttonStop.setEnabled(true);
    currentTree->start(Stream::MODE_LIVE, 0); // TODO: USE MILLIS SINCE EPOCH
  }
  else if (source == &buttonStop)
  {
    currentTree->stop();
    buttonRecord.setEnabled(false);
    buttonStop.setEnabled(false);
  }

  if (source == &ui->sharedGroup->buttonOptions)
  {
    currentTree->stop();
    ui->setScreen(ui->mainScreen);
  }
}
