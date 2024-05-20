#include "PlaybackScreen.h"
#include "UI.h"

using namespace std;

PlaybackScreen::PlaybackScreen(shared_ptr<UI> ui)
:
Screen(ui)
{}

void PlaybackScreen::setCurrentTree(Tree *tree)
{
  currentTree = tree;
}

void PlaybackScreen::resize()
{
  ui->sharedGroup->resize();
  ui->playbackGroup->resize();
}

void PlaybackScreen::run()
{
  currentTree->run();
  ui->sharedGroup->run(currentTree);
  ui->playbackGroup->run(currentTree);
}

void PlaybackScreen::draw()
{
  currentTree->draw();

  ui->beginGroupDraw();
  ui->sharedGroup->draw();
  ui->playbackGroup->draw();
  ui->endGroupDraw();
}

void PlaybackScreen::enter()
{
  ui->sharedGroup->enter(this, currentTree);
  ui->playbackGroup->enter(this, currentTree);

  ui->playbackGroup->buttonPlay.setLocked(true);
  ui->playbackGroup->buttonRewind.setEnabled(true);
  ui->playbackGroup->buttonPause.setEnabled(true);
  currentTree->start(Stream::MODE_PLAYBACK, 0);
}

void PlaybackScreen::event(EventCaster *source, int message)
{
  ui->sharedGroup->event(currentTree, source, message);
  ui->playbackGroup->event(currentTree, source, message);

  if (source == &ui->sharedGroup->buttonOptions)
  {
    currentTree->stop();
    ui->setScreen(ui->mainScreen);
  }
}
