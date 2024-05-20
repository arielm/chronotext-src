#include "Navigator.h"
#include "UI.h"
#include "Branch.h"

#include "chr/cross/Keyboard.h"
#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;

static const u16string ELLIPSE = u"...";

Navigator::Navigator(Tree *tree, std::shared_ptr<UI> ui)
:
tree(tree),
ui(ui)
{
  slot_branches.resize(ui->navigatorSlotCount);
  reset();
}

void Navigator::reset()
{
  slot_active_n = 0;
  over = -1;
  armed = false;
}

void Navigator::mouseIn(int t)
{
  resize();

  armed = (over == -1) && ui->mouse.pressed;
  over = -1;

  for (int i = 0; i < slot_active_n; i++)
  {
    if (boxes[i].contains(ui->mouse.position))
    {
      if (!armed)
      {
        over = i;
      }

      break;
    }
  }

  if ((over != -1) && ui->mouse.pressed)
  {
    tree->stream->navIn(slot_branches[over]->id, t);
  }
}

void Navigator::codeIn(int code, int t)
{
  switch (code)
  {
    case KEY_LEFT:
    {
      auto branch = tree->navHistory.back();

      if (branch)
      {
        tree->stream->navIn(branch->id, t);
      }
    }
    break;

    case KEY_RIGHT :
    {
      auto branch = tree->navHistory.forward();

      if (branch)
      {
        tree->stream->navIn(branch->id, t);
      }
    }
    break;
  }
}

void Navigator::run()
{
  int n = 0;

  for (int i = tree->editHistory.list.size() - 1; i > -1; i--)
  {
    auto branch = tree->editHistory.list[i];

    if (!branch->isEmpty() && (branch != tree->currentBranch))
    {
      slot_branches[n++] = branch;

      if (n == ui->navigatorSlotCount)
      {
        break;
      }
    }
  }

  slot_active_n = n;
}

void Navigator::resize()
{
  float gap = ui->gap * ui->scale;
  float height = ui->navigatorH * ui->scale;
  float width = (ui->windowSize.y - gap * (ui->navigatorSlotCount - 1)) / ui->navigatorSlotCount;
  float x = (ui->windowSize.x - ui->windowSize.y) * 0.5f;
  float y = ui->navigatorY * ui->scale;

  boxes.resize(ui->navigatorSlotCount);

  for (int i = 0; i < ui->navigatorSlotCount; i++)
  {
    boxes[i] = {x + (width + gap) * i, y, width, height};
  }
}

void Navigator::draw()
{
  resize();

  ui->beginGroupDraw();

  for (int i = 0; i < ui->navigatorSlotCount; i++)
  {
    draw::Rect()
      .setColor(0x50 / 255.0f, 0x50 / 255.0f, 0x50 / 255.0f, 1)
      .setBounds(boxes[i])
      .append(ui->flatBatch);
  }

  for (int i = 0; i < slot_active_n; i++)
  {
    ui->subFont->setColor((over == i) ?
      glm::vec4(0xff / 255.0f, 0xff / 255.0f, 0x99 / 255.0f, 1) :
      glm::vec4(0xcc / 255.0f, 0xcc / 255.0f, 0xcc / 255.0f, 1));

    const auto &chars = slot_branches[i]->stream_char;

    if (!chars.empty())
    {
      int size = chars.size();
      float ellipseWidth = ui->subFont->getStringAdvance(ELLIPSE);
      u16string buffer;

      do
      {
        buffer += chars[--size];
        float width = ui->subFont->getStringAdvance(buffer);

        if (width > boxes[i].width() - ui->navigatorPadding * ui->scale * 2 - ellipseWidth)
        {
          buffer.pop_back();
          buffer += ELLIPSE;
          break;
        }
      }
      while (size > 0);

      std::reverse(buffer.begin(), buffer.end());
      ui->drawTextInRect(buffer, boxes[i]);
    }
  }

  ui->endGroupDraw();
}
