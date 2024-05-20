#include "Sketch.h"

using namespace std;
using namespace chr;
using namespace gl;

#if defined(CHR_PLATFORM_EMSCRIPTEN)
  #include <emscripten/bind.h>

  void fileLoaded(uintptr_t instance, const std::string &input)
  {
    reinterpret_cast<Sketch*>(instance)->fileLoaded(input.data(), input.size());
  }

  EMSCRIPTEN_BINDINGS(whatever)
  {
    emscripten::function("fileLoaded", &fileLoaded);
  }

  void Sketch::fileLoaded(const char *data, size_t size)
  {
    ui->playbackScreen->setCurrentTree(treeManager->createLoadedTree(ui, InputSource::buffer(data, size)).get());
    ui->setScreen(ui->playbackScreen);
  }
#endif

void Sketch::setup()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    EM_ASM_ARGS(
    {
      document.getElementById('canvas').addEventListener('click', function(evt)
      {
        var rect = canvas.getBoundingClientRect();
        var mouseX = evt.clientX - rect.left;
        var mouseY = evt.clientY - rect.top;

        if (mouseX >= Module.bounds.x1 && mouseX < Module.bounds.x2 && mouseY >= Module.bounds.y1 && mouseY < Module.bounds.y2)
        {
          input = document.createElement('input');
          input.setAttribute('type', 'file');

          input.onchange = function()
          {
            reader = new FileReader();

            reader.onload = function(loadedEvent)
            {
              Module.fileLoaded($0, loadedEvent.target.result);
            };

            reader.readAsArrayBuffer(this.files[0]);
          };

          input.click();
        }
      }, false);
    }, this);
  #endif

  // ---

  ui = make_shared<UI>();
  ui->setup();

  treeManager = make_shared<TreeManager>();
  ui->mainScreen->setTreeManager(treeManager);

  // ---

  ui->playbackScreen->setCurrentTree(treeManager->createLoadedTree(ui, InputSource::resource("intro.chr")).get());
  ui->setScreen(ui->playbackScreen);

  // ---

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  ui->resize(windowInfo.size);
}

void Sketch::update()
{
  ui->run();
}

void Sketch::draw()
{
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  ui->draw();
}

void Sketch::mouseMoved(float x, float y)
{
  ui->mouseMoved(x, y);
  treeManager->mouseMoved(x, y);
}

void Sketch::mouseDragged(int button, float x, float y)
{
  ui->mouseDragged(button, x, y);
  treeManager->mouseDragged(button, x, y);
}

void Sketch::mousePressed(int button, float x, float y)
{
  ui->mousePressed(button, x, y);
  treeManager->mousePressed(button, x, y);
}

void Sketch::mouseReleased(int button, float x, float y)
{
  ui->mouseReleased(button, x, y);
  treeManager->mouseReleased(button, x, y);
}

void Sketch::keyPressed(uint32_t codepoint)
{
  treeManager->keyPressed(codepoint);
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  treeManager->keyDown(keyCode, modifiers);
}
