#include "Episode3A.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace gl;

const float LINE_H = 8;
const float TERRAIN_H = 150;
const float GRID_SIZE = 4;

const float FILL_POLYGON_OFFSET = 8.0f;
const float TEXT_POLYGON_OFFSET = 0.0f;

const float R1 = 0;
const float R2 = 8;
const float TURNS = 80;
const float H = 200;
const float DD1 = 0.005f;
const float DD2 = 2;

const float BORDER_HM[] = {272, 600};
const float BORDER_HP[] = {720, 144};
const float BORDER_VM[] = {112, 400};
const float BORDER_VP[] = {720, 400};

const glm::vec3 COLOR_BACK(0.9f, 0.9f, 0.9f);
const glm::vec4 COLOR_SOIL(0.0f, 0.0f, 0.0f, 1.0f);
const glm::vec4 COLOR_TEXT_TRACK(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_TEXT_SOIL(COLOR_TEXT_TRACK.r, COLOR_TEXT_TRACK.g, COLOR_TEXT_TRACK.b, 0.4f);
const glm::vec4 COLOR_WIRE_SOIL(COLOR_TEXT_TRACK.r, COLOR_TEXT_TRACK.g, COLOR_TEXT_TRACK.b, 0.4f);
const glm::vec4 COLOR_COLUMN_TEXT_DARK(1.0f, 1.0f, 1.0f, 1.0f);
const glm::vec4 COLOR_COLUMN_TEXT_LIGHT(COLOR_COLUMN_TEXT_DARK.r, COLOR_COLUMN_TEXT_DARK.g, COLOR_COLUMN_TEXT_DARK.b, 0.4f);
const glm::vec4 COLOR_COLUMN_WIRE(COLOR_COLUMN_TEXT_DARK.r, COLOR_COLUMN_TEXT_DARK.g, COLOR_COLUMN_TEXT_DARK.b, 0.4f);

const int COLUMN_N_GROUPS = 2;
const int COLUMN_N_ROWS = 7;
const int COLUMN_N_PER_ROW = 3;
const int COLUMN_N = COLUMN_N_PER_ROW * COLUMN_N_ROWS;

const int COLUMN_ROWS[][COLUMN_N_ROWS] = {{13, 14, 12, 15, 11, 16, 10}, {50, 51, 49, 52, 48, 53, 47}};
const float COLUMN_GROUP_X0[] = {-128, 128};

const float COLUMN_AY = -15 * D2R; // RADIANS

Episode3A::Episode3A(const Episode::Context &context, Episode::Lang lang)
:
Episode(context, lang),
fogManager(context.clock, FogManager::MODE_EXP),
fadeManager(context.clock),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag")),
surface(800, GRID_SIZE, 0.01f, 2, 123456789),
textOX(0),
textOY(0)
{}

Episode3A::~Episode3A()
{
  delete[] patches;
  delete[] sequencesSoil;
}

void Episode3A::setup()
{
  columnPathIndex.assign(COLUMN_N_GROUPS, vector<float>());
  columnY.assign(COLUMN_N_GROUPS, vector<float>());

  for (int i = 0; i < COLUMN_N_GROUPS; i++)
  {
    columnPathIndex[i].reserve(COLUMN_N_ROWS);
    columnY[i].reserve(COLUMN_N_ROWS);
  }

  columnX0.reserve(COLUMN_N);
  columnX.reserve(COLUMN_N);
  columnVX.reserve(COLUMN_N);
  columnAZ.reserve(COLUMN_N);
  columnVA.reserve(COLUMN_N);

  // ---

  switch (lang)
  {
    case LANG_HE:
      fontSize = 8;
      fontSize1 = 1;
      fontSize2 = 6;
      textDir = -1;
      mirror = false;

      font = fontManager.getFont(InputSource::resource("FrankRuehl_Regular_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;

    case LANG_EN:
    case LANG_FR:
      fontSize = 6;
      fontSize1 = 0.75f;
      fontSize2 = 4.5f;
      textDir = +1;
      mirror = true;

      font = fontManager.getFont(InputSource::resource("Georgia_Bold_64.fnt"), XFont::Properties3d().setCapacity(8192));
      break;
  }

  font->setDirection(textDir);

  switch (lang)
  {
    case LANG_HE:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("CommunityRule_09.txt"));
      line = utils::readText<u16string>(InputSource::resource("Sanctuary.txt"));
      break;

    case LANG_EN:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("CommunityRule_09_EN.txt"));
      line = utils::readText<u16string>(InputSource::resource("Sanctuary_EN.txt"));
      break;

    case LANG_FR:
      lineProvider = make_shared<LineProvider>(*font, fontSize, InputSource::resource("CommunityRule_09_FR.txt"));
      line = utils::readText<u16string>(InputSource::resource("Sanctuary_FR.txt"));
      break;
  }

  // ---

  surface.generateBase();

  /*
   * C++ "PLACEMENT NEW":
   * http://stackoverflow.com/a/2493450/50335
   */
  patches = new TextNoisePatch[2];
  sequencesSoil = new xf::FontSequence[2];

  font->setAxis(mirror ? -1 : +1, -1); // XXX

  for (int i = 0; i < 2; i++)
  {
    new (&patches[i]) TextNoisePatch(&surface, TEXT_POLYGON_OFFSET, font, lineProvider, LINE_H);
    patches[i].setup(-BORDER_HM[i], -BORDER_VM[i], BORDER_HM[i] + BORDER_HP[i], BORDER_VM[i] + BORDER_VP[i], NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H);
    patches[i].update(TERRAIN_H, glm::vec2(0));

    font->beginSequence(sequencesSoil[i]);
    patches[i].drawText(-BORDER_HM[i], -BORDER_VM[i], BORDER_HM[i] + BORDER_HP[i], BORDER_VM[i] + BORDER_VP[i], textOX, textOY, TERRAIN_H);
    font->endSequence();
  }

  allocateColumnGroup(0, 0);
  allocateColumnGroup(1, 1);

  // ---

  helix = Helix(font);
  helix.update(R1, R2, TURNS, H, DD1, DD2);

  font->setAxis(mirror ? + 1 : -1, +1); // XXX

  font->beginSequence(sequenceColumn);
  helix.drawText(line, R1, R2, TURNS, H, 0, fontSize1, fontSize2);
  font->endSequence();

  // ---

  view = View(clock, 60 * D2R, 1, 2000, mirror);
  view.resize(windowSize); // XXX
  view.add(glm::quat(0.8224593f, -0.52512515f, 0.21844135f, -0.009369576f), 166, glm::vec3(0, 0, 0));
  view.add(glm::quat(0.48125312f, -0.36623877f, -0.50510263f, -0.61574024f), 186, glm::vec3(1, 0, 0));
  view.add(glm::quat(0.47737357f, -0.36964974f, -0.51079434f, -0.61201537f), 182, glm::vec3(60, 0, 0));

  fogManager.add(COLOR_BACK, 0.00333f);

  fadeManager.resize(windowSize); // XXX
}

void Episode3A::enter()
{
  step = 0;

  view.set(0);
  fogManager.set(0);
  fadeManager.reset();
  fadeManager.interpolate(glm::vec3(0, 0, 0), FadeManager::FADE_IN, 2);

  // ---

  currentColumnGroupIndex = 0;
  currentSoilIndex = 0;
  resetColumns(currentColumnGroupIndex);

  // ---

  font->setShader(fogTextureAlphaShader);
}

void Episode3A::update()
{
  switch (step)
  {
    case 0 :
      if (clock->getTime() >= 15.750)
      {
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_OUT, 0.25);
        step = 1;
      }
      break;

    case 1 :
      if (clock->getTime() >= 16)
      {
        currentColumnGroupIndex = 1;
        currentSoilIndex = 1;
        resetColumns(currentColumnGroupIndex);

        view.interpolate(1, 2, 18);
        fadeManager.interpolate(COLOR_BACK, FadeManager::FADE_IN, 0.25);
        step = 2;
      }
      break;

    case 2 :
      if (clock->getTime() >= 32)
      {
        fadeManager.interpolate(glm::vec3(1, 1, 1), FadeManager::FADE_OUT, 2);
        step = 3;
      }
      break;

    case 3 :
      if (clock->getTime() >= 35)
      {
        nextEpisode();
      }
      break;
  }

  for (int i = 0; i < COLUMN_N; i++)
  {
    double t = clock->getTime() - ((step >=2) ? 16 : 0);

    columnX[i] = columnX0[i] + columnVX[i] * t;
    columnAZ[i] = columnVA[i] * t;
  }
}

void Episode3A::draw()
{
  glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
  glDepthMask(true);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  glm::mat4x4 projectionMatrix;
  Matrix modelViewMatrix;
  tie(projectionMatrix, modelViewMatrix) = view.run();

  modelViewMatrix.translate(0, 0, -(surface.noiseMin + (surface.noiseMax - surface.noiseMin) * 0.5f) * TERRAIN_H);

  // ---

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

  drawSoil(modelViewMatrix, projectionMatrix, currentSoilIndex);

  font->setColor(COLOR_TEXT_SOIL);
  font->replaySequence(sequencesSoil[currentSoilIndex]);

  drawTracks(currentColumnGroupIndex, currentSoilIndex);
  drawColumns(modelViewMatrix, projectionMatrix, currentColumnGroupIndex, currentSoilIndex);

  // ---

  fadeManager.run();
}

void Episode3A::drawSoil(Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix, int soilIndex)
{
  State state;
  state
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix);

  fogManager.run(state);
  state.apply();

  glFrontFace(mirror ? GL_CCW : GL_CW);

  glDepthMask(true);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(FILL_POLYGON_OFFSET, 1.0f);

  patches[soilIndex].drawFill(COLOR_SOIL);

  glDisable(GL_POLYGON_OFFSET_FILL);
  glDepthMask(false);

  patches[soilIndex].drawGrid(COLOR_WIRE_SOIL);
}

void Episode3A::drawTracks(int columnGroupIndex, int soilIndex)
{
  font->setColor(COLOR_TEXT_TRACK.r, COLOR_TEXT_TRACK.g, COLOR_TEXT_TRACK.b, COLOR_TEXT_TRACK.a / COLUMN_N_PER_ROW);
  font->setSize(fontSize);
  font->setAxis(mirror ? -1 : +1, -1); // XXX
  
  font->beginSequence(sequenceTracks);

  for (int i = 0; i < COLUMN_N; i++)
  {
    if (columnX[i] > -BORDER_HM[columnGroupIndex])
    {
      int row = i % COLUMN_N_ROWS;
      drawTrack(columnGroupIndex, soilIndex, row, columnX[i]);
    }
  }

  font->endSequence();
  font->replaySequence(sequenceTracks);
}

void Episode3A::drawTrack(int columnGroupIndex, int soilIndex, int row, float x)
{
  math::Rectf clip(glm::vec2(-99999, -99999), glm::vec2(x, +99999));
  patches[soilIndex].drawLine(columnPathIndex[columnGroupIndex][row], -BORDER_HM[soilIndex], x, textOX, textOY, TERRAIN_H, clip);
}

void Episode3A::drawColumns(Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix, int columnGroupIndex, int soilIndex)
{
  for (int i = 0; i < COLUMN_N; i++)
  {
    if (columnX[i] > -BORDER_HM[columnGroupIndex])
    {
      int row = i % COLUMN_N_ROWS;
      drawColumn(modelViewMatrix, projectionMatrix, columnGroupIndex, soilIndex, columnX[i], columnY[columnGroupIndex][row], columnAZ[i]);
    }
  }
}

void Episode3A::drawColumn(Matrix &modelViewMatrix, glm::mat4x4 &projectionMatrix, int columnGroupIndex, int soilIndex, float x, float y, float az)
{
  Matrix::Stack stack;

  modelViewMatrix
    .push(stack)
    .translate(x, y, TERRAIN_H * surface.getHeight(x, y))
    .rotateY(COLUMN_AY)
    .rotateZ(az);

  State()
    .setShader(fogColorShader)
    .setShaderMatrix(modelViewMatrix * projectionMatrix)
    .apply();

  modelViewMatrix.pop(stack);

  helix.wireBatch
    .setPrimitive(GL_LINE_STRIP)
    .setShaderColor(COLOR_COLUMN_WIRE)
    .flush();

  glFrontFace(mirror ? GL_CCW : GL_CW);

  font->setColor(COLOR_COLUMN_TEXT_DARK);
  font->replaySequence(sequenceColumn);

  glFrontFace(mirror ? GL_CW : GL_CCW);

  font->setColor(COLOR_COLUMN_TEXT_LIGHT);
  font->replaySequence(sequenceColumn);
}

void Episode3A::resetColumns(int columnGroupIndex)
{
  JavaRandom rnd(1153157381796);

  float x0 = COLUMN_GROUP_X0[columnGroupIndex];
  int n = 0;

  for (int j = 0; j < COLUMN_N_PER_ROW; j++)
  {
    for (int i = 0; i < COLUMN_N_ROWS; i++)
    {
      columnX0[n] = x0 - rnd.nextFloat() * 30;
      columnVX[n] = 4.5f - 0.9f + 1.8f * rnd.nextFloat();
      columnVA[n] = (columnVX[n] / R2); // RADIANS
      n++;
    }

    x0 -= (1 + rnd.nextFloat()) * 30;
  }
}

void Episode3A::allocateColumnGroup(int columnGroupIndex, int soilIndex)
{
  float ym = -BORDER_VM[soilIndex];
  float yp = +BORDER_VP[soilIndex];

  float baselineOffset = -font->getOffsetY(XFont::ALIGN_MIDDLE);
  float minus = ym + (baselineOffset + font->getAscent()) - fmodf(textOY, LINE_H);
  float plus = yp - (baselineOffset - font->getDescent()) - fmodf(textOY, LINE_H);

  int total_m = int(minus / LINE_H);
  int total_p = int(plus / LINE_H);

  // ---

  int n1 = 0;
  int n2 = 0;

  for (int i = total_m; i <= total_p; i++)
  {
    for (int j = 0; j < COLUMN_N_ROWS; j++)
    {
      if (COLUMN_ROWS[columnGroupIndex][j] == n1)
      {
        columnPathIndex[columnGroupIndex][n2] = i;
        columnY[columnGroupIndex][n2] = i * LINE_H + fmodf(textOY, LINE_H);
        n2++;
      }
    }

    n1++;
  }
}
