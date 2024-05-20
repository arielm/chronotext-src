#include "MainScreen.h"
#include "UI.h"
#include "TreeManager.h"

#include "nfd/nfd.h"
#include "chr/cross/Context.h"

using namespace std;
using namespace chr;
using namespace math;

static const u16string CREDITS = u"the text time curvature\nariel malka | june 2004 | chronotext.org\nmade for processing.org/exhibition";

MainScreen::MainScreen(shared_ptr<UI> ui)
:
Screen(ui)
{
  buttonLoad = TextButton(ui, u"LOAD & PLAY NEW");
  buttonPlayLoaded = TextButton(ui, u"REPLAY IT");
  buttonRecordNew = TextButton(ui, u"RECORD NEW");
  buttonPlayRecorded = TextButton(ui, u"REPLAY IT");
  buttonSave = TextButton(ui, u"SAVE IT");
  buttonAbout = TextButton(ui, u"ABOUT + HELP");

  //

  textBox.setText(CREDITS);
  textBox.setAutoHeight(true);
  textBox.setFont(ui->subFont);
  textBox.setLineHeightFactor(1.2f);
  textBox.setTextColor(glm::vec4(1, 1, 1, 1)); // XXX
}

void MainScreen::setTreeManager(shared_ptr<TreeManager> treeManager)
{
  this->treeManager = treeManager;
}

void MainScreen::resize()
{
  float x = (ui->windowSize.x - ui->windowSize.y) * 0.5f;
  float padding = 16 * ui->scale;
  float mainButtonX = x + 283 * ui->scale;
  float mainButtonW = 101 * ui->scale;
  float mainButtonH = ui->deckH * ui->scale;

  //

  buttonLoad.setBounds(Rectf(mainButtonX, 47 * ui->scale, mainButtonW, mainButtonH));
  buttonPlayLoaded.setBounds(Rectf(mainButtonX, 71 * ui->scale, mainButtonW, mainButtonH));
  buttonRecordNew.setBounds(Rectf(mainButtonX, 169 * ui->scale, mainButtonW, mainButtonH));
  buttonPlayRecorded.setBounds(Rectf(mainButtonX, 193 * ui->scale, mainButtonW, mainButtonH));
  buttonSave.setBounds(Rectf(mainButtonX, 217 * ui->scale, mainButtonW, mainButtonH));
  buttonAbout.setBounds(Rectf(mainButtonX, 326 * ui->scale, mainButtonW, mainButtonH));

  //

  textBox.setFontSize(ui->subFontSize * ui->scale);
  textBox.setWidth(251 * ui->scale);

  textBox.layout();
  textBox.setLocation(x + padding, ui->windowSize.y - padding - textBox.getHeight());

  //

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    auto bounds = buttonLoad.bounds;

    EM_ASM_({
      Module.bounds = {};
      Module.bounds.x1 = $0;
      Module.bounds.y1 = $1;
      Module.bounds.x2 = $2;
      Module.bounds.y2 = $3;
    }, bounds.x1, bounds.y1, bounds.x2, bounds.y2);
  #endif
}

void MainScreen::run()
{
  buttonLoad.run();
  buttonPlayLoaded.run();
  buttonRecordNew.run();
  buttonPlayRecorded.run();
  buttonSave.run();
  buttonAbout.run();
}

void MainScreen::draw()
{
  ui->beginGroupDraw();

  //

  glm::vec4 color(0x50 / 255.0f, 0x50 / 255.0f, 0x50 / 255.0f, 1);

  float x1 = (ui->windowSize.x - ui->windowSize.y) * 0.5f;
  float x2 = x1 + ui->windowSize.y;
  float top = 0;
  float bottom = ui->windowSize.y;

  ui->lineBatch
    .addVertex(x1 + 267 * ui->scale, top, color).addVertex(x1 + 267 * ui->scale, bottom, color)
    .addVertex(x1 + 267 * ui->scale, 133 * ui->scale, color).addVertex(x2, 133 * ui->scale, color)
    .addVertex(x1 + 267 * ui->scale, 267 * ui->scale, color).addVertex(x2, 267 * ui->scale, color)
    .addVertex(x2, top, color).addVertex(x2, bottom, color);

  //

  buttonLoad.draw();
  buttonPlayLoaded.draw();
  buttonRecordNew.draw();
  buttonPlayRecorded.draw();
  buttonSave.draw();
  buttonAbout.draw();

  ui->endGroupDraw();

  // ---

  textBox.draw();
}

void MainScreen::enter()
{
  buttonLoad.setListener(this);
  buttonLoad.reset();

  buttonPlayLoaded.setListener(this);
  buttonPlayLoaded.reset();

  buttonRecordNew.setListener(this);
  buttonRecordNew.reset();

  buttonPlayRecorded.setListener(this);
  buttonPlayRecorded.reset();

  buttonSave.setListener(this);
  buttonSave.reset();

  buttonAbout.setListener(this);
  buttonAbout.reset();

  if (!treeManager->streamLoaded || !treeManager->streamLoaded->loaded)
  {
    buttonPlayLoaded.setEnabled(false);
  }

  if (!treeManager->streamRecorded || !treeManager->streamRecorded->recorded)
  {
    buttonPlayRecorded.setEnabled(false);
    buttonSave.setEnabled(false);
  }
}

void MainScreen::event(EventCaster *source, int message)
{
  if (source == &buttonLoad)
  {
    #if defined(CHR_PLATFORM_DESKTOP)
      nfdchar_t *path = nullptr;
      nfdresult_t result = NFD_OpenDialog("chr", nullptr, &path);

      if (result == NFD_OKAY)
      {
        auto inputSource = InputSource::file(path);
        free(path);

        ui->playbackScreen->setCurrentTree(treeManager->createLoadedTree(ui, inputSource).get());
        ui->setScreen(ui->playbackScreen);
      }
    #endif
  }
  else if (source == &buttonPlayLoaded)
  {
    ui->playbackScreen->setCurrentTree(treeManager->treeLoaded.get());
    ui->setScreen(ui->playbackScreen);
  }
  else if (source == &buttonRecordNew)
  {
    ui->liveScreen->setCurrentTree(treeManager->createRecordedTree(ui).get());
    ui->setScreen(ui->liveScreen);
  }
  else if (source == &buttonPlayRecorded)
  {
    ui->playbackScreen->setCurrentTree(treeManager->treeRecorded.get());
    ui->setScreen(ui->playbackScreen);
  }
  else if (source == &buttonSave)
  {
    #if defined (CHR_PLATFORM_DESKTOP)
      nfdchar_t *path = nullptr;
      nfdresult_t result = NFD_SaveDialog("chr", nullptr, &path);

      if (result == NFD_OKAY)
      {
        auto outputTarget = OutputTarget::file(path);
        treeManager->streamRecorded->save(outputTarget);
        free(path);
      }
    #elif defined(CHR_PLATFORM_EMSCRIPTEN)
      auto outputTarget = OutputTarget::buffer();
      treeManager->streamRecorded->save(outputTarget);

      EM_ASM_ARGS(
      {
        var buffer = HEAPU8.subarray($0, $0 + $1);
        var blob = new Blob([buffer], {type: 'application/octet-binary'});
        saveAs(blob, "record.chr");
      }, outputTarget.getData(), outputTarget.getDataSize());
    #endif
  }
  else if (source == &buttonAbout)
  {
    ui->setScreen(ui->aboutScreen);
  }
}
