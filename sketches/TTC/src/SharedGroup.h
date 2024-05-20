#pragma once

#include "Group.h"
#include "Tree.h"
#include "Screen.h"
#include "ImageButton.h"
#include "TextButton.h"
#include "TextItem.h"

class SharedGroup : public Group
{
public:
  TextItem itemViews;
  TextButton buttonView1, buttonView2, buttonView3;
  ImageButton buttonOptions;
  TextItem itemOptions;

  SharedGroup(std::shared_ptr<UI> ui);

  void resize() final;
  void draw() final;

  void enter(Screen *screen, Tree *tree);
  void run(Tree *tree);
  void event(Tree *tree, EventCaster *source, int message);

protected:
  void setTreeView1(Tree *tree);
  void setTreeView2(Tree *tree);
  void setTreeView3(Tree *tree);
};
