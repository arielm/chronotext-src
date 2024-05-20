#include "common/legacy/ui/ButtonLayout.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

static constexpr float GRACE_PERIOD = 2;

void ButtonLayout::setup(UILock::Ref uiLock, const TiledAtlas &atlas, int padding)
{
  this->uiLock = uiLock;
  this->atlas = atlas;
  this->padding = padding;

  //

  fillBatch.setShader(colorShader);

  textureBatch
    .setShader(textureAlphaShader)
    .setTexture(atlas.getTexture());

  //

  clock = Clock::create();
  clock->start();
}

void ButtonLayout::resize(const glm::vec2 &size)
{
  windowSize = size;

  for (auto it = cartridges.begin(); it!= cartridges.end(); ++it)
  {
    switch (it->first)
    {
      case Layout::LM:
      {
        float x = padding;
        float y = (size.y - getDimension(it->second)) * 0.5f;

        layoutVertically(it->second, x, y);
      }
      break;

      case Layout::RM:
      {
        float x = size.x - padding - it->second.front().getSize().x;
        float y = (size.y - getDimension(it->second)) * 0.5f;

        layoutVertically(it->second, x, y);
      }
      break;
    }
  }
}

void ButtonLayout::update()
{
  if (!uiLock->check(this))
  {
    if (clock->getTime() > GRACE_PERIOD)
    {
      buttonsHidden = true;
    }
  }}

void ButtonLayout::draw()
{
  auto mvpMatrix = glm::ortho(0.0f, windowSize.x, windowSize.y, 0.0f);

  State()
    .glEnable(GL_BLEND)
    .glDisable(GL_CULL_FACE)
    .glDisable(GL_DEPTH_TEST)
    .setShaderMatrix(mvpMatrix)
    .apply();

  if (!buttonsHidden)
  {
    fillBatch.clear();
    textureBatch.clear();

    for (auto it = cartridges.begin(); it != cartridges.end(); ++it)
    {
      for (auto &button : it->second)
      {
        button.draw(fillBatch, textureBatch);
      }
    }

    fillBatch.flush();
    textureBatch.flush();
  }
}

void ButtonLayout::setHandler(Button::Handler *handler)
{
  this->handler = handler;
}

void ButtonLayout::addButton(Layout layout, const Button &button)
{
  cartridges[layout].push_back(button);
  layoutMap[button.id] = layout;
}

Button* ButtonLayout::getButton(int id)
{
  auto found = layoutMap.find(id);

  if (found != layoutMap.end())
  {
    auto layout = found->second;

    for (auto &button : cartridges[layout])
    {
      if (button.id == id)
      {
        return &button;
      }
    }
  }

  return nullptr;
}

void ButtonLayout::toggleButton(int id)
{
  auto found = layoutMap.find(id);

  if (found != layoutMap.end())
  {
    auto layout = found->second;

    for (auto &button : cartridges[layout])
    {
      if (button.id == id)
      {
        handleButtonToggled(&button);
        button.state = Button::STATE_TOGGLED;
      }
      else
      {
        button.state = Button::STATE_NORMAL;
      }
    }
  }
}

void ButtonLayout::mouseMoved(float x, float y)
{
  clock->restart();
  buttonsHidden = false;
}

void ButtonLayout::mouseDragged(int button, float x, float y)
{
  Button *armedButton = getArmedButtonByIndex(button);

  if (armedButton && (armedButton->type == Button::TYPE_CLICKABLE))
  {
    if (armedButton == getClosestButton(glm::vec2(x, y)))
    {
      armedButton->changeState(Button::STATE_PRESSED);
    }
    else
    {
      armedButton->changeState(Button::STATE_NORMAL);
    }
  }
}

void ButtonLayout::mousePressed(int button, float x, float y)
{
  if (!buttonsHidden)
  {
    Button *closestButton = getClosestButton(glm::vec2(x, y));

    if (closestButton && uiLock->acquire(this))
    {
      switch (closestButton->type)
      {
        case Button::TYPE_CLICKABLE:
          if (closestButton->armedIndex == -1)
          {
            closestButton->armedIndex = button;
            closestButton->changeState(Button::STATE_PRESSED);
          }
          break;

        case Button::TYPE_PRESSABLE:
          closestButton->armedIndex = button;
          closestButton->changeState(Button::STATE_PRESSED);

          handleButtonPressed(closestButton);
          break;

        case Button::TYPE_TOGGLABLE:
          if (closestButton->state == Button::STATE_NORMAL)
          {
            toggleButton(closestButton->id);
          }
          break;
      }
    }
  }
}

void ButtonLayout::mouseReleased(int button, float x, float y)
{
  if (uiLock->release(this))
  {
    Button *armedButton = getArmedButtonByIndex(button);

    if (armedButton)
    {
      if ((armedButton->type == Button::TYPE_CLICKABLE) && (armedButton->state == Button::STATE_PRESSED))
      {
        handleButtonClicked(armedButton);
      }

      armedButton->state = Button::STATE_NORMAL;
      armedButton->armedIndex = -1;
    }

    clock->restart();
  }
}

float ButtonLayout::getDimension(vector<Button> &cartridge)
{
  auto count = cartridge.size();

  if (count > 0)
  {
    float buttonSize = cartridge.front().getSize().x;
    return count * buttonSize + (count - 1) * padding;
  }
  else
  {
    return 0;
  }
}

void ButtonLayout::layoutVertically(vector<Button> &cartridge, float x, float y)
{
  float buttonSize = cartridge.front().getSize().x;

  for (auto &button : cartridge)
  {
    button.setBounds(Rectf(x, y, buttonSize, buttonSize));
    y += buttonSize + padding;
  }
}

void ButtonLayout::handleButtonClicked(Button *button)
{
  if (handler)
  {
    handler->buttonEvent(button->id, Button::ACTION_CLICKED);
  }
}

void ButtonLayout::handleButtonPressed(Button *button)
{
  if (handler)
  {
    handler->buttonEvent(button->id, Button::ACTION_PRESSED);
  }
}

void ButtonLayout::handleButtonToggled(Button *button)
{
  if (handler)
  {
    handler->buttonEvent(button->id, Button::ACTION_TOGGLED);
  }
}

Button* ButtonLayout::getClosestButton(const glm::vec2 &position)
{
  Button *closestButton = nullptr;
  float closestDistance = FLT_MAX;

  for (auto it = cartridges.begin(); it!= cartridges.end(); ++it)
  {
    for (auto &button : it->second)
    {
      float distance;

      if (button.hitTest(position, &distance))
      {
        if (distance < closestDistance)
        {
          closestDistance = distance;
          closestButton = &button;
        }
      }
    }
  }

  return closestButton;
}

Button* ButtonLayout::getArmedButtonByIndex(int index)
{
  for (auto it = cartridges.begin(); it!= cartridges.end(); ++it)
  {
    for (auto &button : it->second)
    {
      if (button.armedIndex == index)
      {
        return &button;
      }
    }
  }

  return nullptr;
}
