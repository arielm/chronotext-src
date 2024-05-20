#include "SharedGroup.h"
#include "UI.h"

using namespace std;
using namespace chr;
using namespace math;

SharedGroup::SharedGroup(std::shared_ptr<UI> ui)
:
Group(ui)
{
  itemViews = TextItem(ui, u"views", XFont::ALIGN_LEFT);
  buttonView1 = TextButton(ui, u"1");
  buttonView2 = TextButton(ui, u"2");
  buttonView3 = TextButton(ui, u"3");

  itemOptions = TextItem(ui, u"options", XFont::ALIGN_RIGHT);
  buttonOptions = ImageButton(ui, "options");
}

void SharedGroup::resize()
{
  float gap = ui->gap * ui->scale;
  float height = ui->deckH * ui->scale;
  float width = ui->windowSize.y;
  float x = (ui->windowSize.x - ui->windowSize.y) * 0.5f;
  float y = ui->deckY * ui->scale;

  float x1 = x + gap;
  float x2 = x1 + gap + itemViews.getWidth();
  float x3 = x2 + gap + height;
  float x4 = x3 + gap + height;

  float x5 = x + width - gap - height;
  float x6 = x5 - gap;

  //

  itemViews.setPosition(glm::vec2(x1, y + height * 0.5f));
  buttonView1.setBounds(Rectf(x2, y, height, height));
  buttonView2.setBounds(Rectf(x3, y, height, height));
  buttonView3.setBounds(Rectf(x4, y, height, height));

  itemOptions.setPosition(glm::vec2(x6, y + height * 0.5f));
  buttonOptions.setBounds(Rectf(x5, y, height, height));
}

void SharedGroup::draw()
{
  itemViews.draw();
  buttonView1.draw();
  buttonView2.draw();
  buttonView3.draw();

  itemOptions.draw();
  buttonOptions.draw();
}

void SharedGroup::enter(Screen *screen, Tree *tree)
{
  buttonView1.setListener(screen);
  buttonView1.reset();
  buttonView1.setLocked(true);
  setTreeView1(tree);

  buttonView2.setListener(screen);
  buttonView2.reset();

  buttonView3.setListener(screen);
  buttonView3.reset();

  buttonOptions.setListener(screen);
  buttonOptions.reset();
}

void SharedGroup::run(Tree *tree)
{
  buttonView1.run();
  buttonView2.run();
  buttonView3.run();
  buttonOptions.run();
}

void SharedGroup::event(Tree *tree, EventCaster *source, int message)
{
  if (source == &buttonView1)
  {
    buttonView1.setLocked(true);
    buttonView2.setLocked(false);
    buttonView3.setLocked(false);
    setTreeView1(tree);
  }
  else if (source == &buttonView2)
  {
    buttonView2.setLocked(true);
    buttonView1.setLocked(false);
    buttonView3.setLocked(false);
    setTreeView2(tree);
  }
  else if (source == &buttonView3)
  {
    buttonView3.setLocked(true);
    buttonView1.setLocked(false);
    buttonView2.setLocked(false);
    setTreeView3(tree);
  }
}

// ---

void SharedGroup::setTreeView1(Tree *tree)
{
  tree->cam_fov = 60;
  tree->cam_twist = 0 * D2R;
  tree->cam_elevation = 330 * D2R;
  tree->cam_azimuth = 0 * D2R;
  tree->cam_x = 0;
  tree->cam_y = 0;
  tree->cam_distance = 275;
}

void SharedGroup::setTreeView2(Tree *tree)
{
  tree->cam_fov = 60;
  tree->cam_twist = 0 * D2R;
  tree->cam_elevation = -60 * D2R;
  tree->cam_azimuth = 23 * D2R;
  tree->cam_x = 50;
  tree->cam_y = 10;
  tree->cam_distance = 158;
}

void SharedGroup::setTreeView3(Tree *tree)
{
  tree->cam_fov = 60;
  tree->cam_twist = 0 * D2R;
  tree->cam_elevation = 0 * D2R;
  tree->cam_azimuth = 0 * D2R;
  tree->cam_x = 0;
  tree->cam_y = 0;
  tree->cam_distance = 500;
}
