#pragma once

#include "Screen.h"
#include "TextButton.h"

#include "common/legacy/ui/TextBox.h"

class TreeManager;

class MainScreen : public Screen
{
public:
  MainScreen(std::shared_ptr<UI> ui);

  void setTreeManager(std::shared_ptr<TreeManager> treeManager);

  void resize() final;
  void run() final;
  void draw() final;
  void enter() final;

  void event(EventCaster *source, int message) final;

protected:
  std::shared_ptr<TreeManager> treeManager;

  TextButton buttonLoad, buttonPlayLoaded;
  TextButton buttonRecordNew, buttonPlayRecorded, buttonSave;
  TextButton buttonAbout;

  chr::TextBox textBox;
};
