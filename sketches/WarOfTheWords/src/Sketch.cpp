#include "Sketch.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

static constexpr float FADE_DURATION = 2;

Sketch::Sketch()
:
explosionShader(InputSource::resource("ExplosionShader.vert"), InputSource::resource("ExplosionShader.frag"))
{}

void Sketch::setup()
{
  processor.init(clock());
  processor.process("war.xml", fontManager);

  // ---

  initButtonLayout();
  initTextures();

  context.flatBatch
    .setShader(colorShader)
    .setShaderColor(1, 0.5f, 0, 1);

  context.textureBatch
    .setShader(textureAlphaShader)
    .setShaderColor(0, 0, 0, TextSpan::textureOpacity)
    .setTexture(context.texture);

  context.lineBatch
    .setPrimitive(GL_LINES)
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 0.1f);

  context.missileBatch
    .setPrimitive(GL_TRIANGLE_STRIP)
    .setShader(colorShader);

  context.explosionBatch
    .setShader(explosionShader);

  // ---

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  buttonLayout.resize(windowInfo.size);
  processor.cameraManager.resize(windowInfo.size);
}

void Sketch::update()
{
  buttonLayout.update();

  if (!paused)
  {
    float t = processor.clock->getTime();

    switch (step)
    {
      case 0:
      {
        if (processor.cameraManager.hasReachedEnd(FADE_DURATION))
        {
          fadeT = t;
          step = 1;
        }
      }
        break;

      case 1:
      {
        fadeFactor = constrainf((t - fadeT) / FADE_DURATION, 0, 1);

        if (fadeFactor == 1)
        {
          fadeT = t;
          step = 2;
        }
      }
        break;

      case 2:
      {
        if (t - fadeT > 1)
        {
          reset();
        }
      }
    }

    processor.run();
  }
}

void Sketch::draw()
{
  glClearColor(1, 1, 1, 1);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  State()
    .setShaderMatrix(processor.cameraManager.getMVPMatrix())
    .apply();

  context.flatBatch.clear();

  draw::Rect rect;
  rect
    .setColor(1, 1, 1, 1)
    .setBounds(processor.article->getBounds())
    .append(context.flatBatch, Matrix().translate(0, 0, -0.01f));

  processor.article->drawWalls(context);

  context.flatBatch.flush();

  //

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDepthMask(GL_FALSE);

  State()
    .setShaderMatrix(processor.cameraManager.getMVPMatrix())
    .setShader(textureAlphaShader)
    .apply();

  processor.article->drawText(context);

  //

  State()
    .setShaderMatrix(processor.cameraManager.getMVPMatrix())
    .apply();

  context.textureBatch.clear();
  processor.article->drawTextures(context);
  context.textureBatch.flush();

  context.lineBatch.clear();
  processor.article->drawOutlines(context);
  context.lineBatch.flush();

  context.missileBatch.clear();
  processor.searchManager.missileManager.draw(context);
  context.missileBatch.flush();

  //

  glEnable(GL_CULL_FACE);

  processor.searchManager.missileManager.explosionHelper.flush(context);

  // ---

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  if (fadeFactor > 0)
  {
    auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

    State()
      .setShaderMatrix(projectionMatrix)
      .apply();

    context.flatBatch.clear();

    rect
      .setColor(1, 1, 1, fadeFactor)
      .setBounds(glm::vec2(0), windowInfo.size)
      .append(context.flatBatch);

    context.flatBatch.flush();
  }

  // ---

  buttonLayout.draw();
}

void Sketch::initTextures()
{
  context.texture = Texture(
    Texture::ImageRequest("camo.jpg")
      .setFlags(image::FLAGS_TRANSLUCENT)
      .setMipmap(true)
      .setAnisotropy(true)
      .setWrap(GL_REPEAT, GL_REPEAT));
}

void Sketch::reset()
{
  step = 0;
  fadeFactor = 0;

  processor.reset();
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  #if defined(CHR_PLATFORM_DESKTOP)
    if (keyCode == KEY_SPACE)
    {
      paused ^= true;

      if (paused)
      {
        processor.clock->stop();
      }
      else
      {
        processor.clock->start();
      }
    }
  #endif
}

void Sketch::mouseMoved(float x, float y)
{
  buttonLayout.mouseMoved(x, y);
}

void Sketch::mouseDragged(int button, float x, float y)
{
  if (uiLock->check(&buttonLayout))
  {
    buttonLayout.mouseDragged(button, x, y);
  }
}

void Sketch::mousePressed(int button, float x, float y)
{
  buttonLayout.mousePressed(button, x, y);
}

void Sketch::mouseReleased(int button, float x, float y)
{
  buttonLayout.mouseReleased(button, x, y);
  uiLock->release(this);
}

void Sketch::buttonEvent(int id, Button::Action action)
{
  if (id == 0)
  {
    if (fullScreen)
    {
      exitFullScreen();
    }
    else
    {
      enterFullScreen();
    }
  }
}

void Sketch::enterFullScreen()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    emscripten_request_fullscreen("#canvas", 1);
  #endif
}

void Sketch::exitFullScreen()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    emscripten_exit_fullscreen();
  #endif
}

void Sketch::fullScreenEvent(bool isFullScreen)
{
  fullScreen = isFullScreen;

  Button *button = buttonLayout.getButton(0);

  if (button)
  {
    if (isFullScreen)
    {
      button->setType(Button::TYPE_CLICKABLE);
      button->setTextureBounds(buttonLayout.atlas.getTileBounds(1, 0));
    }
    else
    {
      button->setType(Button::TYPE_PRESSABLE);
      button->setTextureBounds(buttonLayout.atlas.getTileBounds(0, 0));
    }
  }
}

#if defined(CHR_PLATFORM_EMSCRIPTEN)
  EM_BOOL Sketch::fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData)
  {
    reinterpret_cast<Sketch*>(userData)->fullScreenEvent(e->isFullscreen);
    return 0;
  }
#endif

void Sketch::initButtonLayout()
{
  uiLock = make_shared<UILock>();

  TiledAtlas atlas("atlas1.png", 64, 4);

  buttonLayout.setup(uiLock, atlas, 12);
  buttonLayout.setHandler(this);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    fullScreenEnabled = true;
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, 1, fullScreenChangeCallback);
  #endif

  //

  if (fullScreenEnabled)
  {
    Button::Style style1;
    style1.backgroundColors[Button::STATE_NORMAL] = { 1, 0, 0, 1 };
    style1.colors[Button::STATE_NORMAL] = { 1, 1, 1, 1 };
    style1.backgroundColors[Button::STATE_PRESSED] = { 0, 0, 0, 1 };
    style1.colors[Button::STATE_PRESSED] = { 1, 1, 1, 1 };

    Button buttonR1;
    buttonR1
      .setId(0)
      .setStyle(style1)
      .setTextureBounds(atlas.getTileBounds(0, 0))
      .setType(Button::TYPE_PRESSABLE);
    buttonLayout.addButton(ButtonLayout::RM, buttonR1);
  }
}
