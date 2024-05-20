#pragma once

#include "Screen.h"
#include "ImageButton.h"
#include "TextItem.h"

#include "common/legacy/ui/TextBox.h"

class AboutScreen : public Screen
{
public:
  AboutScreen(std::shared_ptr<UI> ui);

  void resize() final;
  void run() final;
  void draw() final;
  void enter() final;

  void event(EventCaster *source, int message) final;

protected:
  ImageButton buttonOptions;
  TextItem itemOptions;

  chr::TextBox textBox;
};
