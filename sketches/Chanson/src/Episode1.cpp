#include "Episode1.h"
#include "Sketch.h"

using namespace std;
using namespace chr;

constexpr float R1 = 6;
constexpr float R2 = 150;
constexpr float TURNS = 20;
constexpr float DD1 = 0.5f;
constexpr float DD2 = 5;

constexpr float SWELL_FACTOR = 0.125f;
constexpr float FONT_SIZE = 4;
constexpr float BRICK_HEIGHT_RATIO = 1.5f; /* RELATIVE TO FONT-SIZE */
constexpr float BRICK_DEPTH_RATIO = 0.5f; /* RELATIVE TO BRICK HEIGHT */
constexpr float POLYGON_OFFSET = 0.125f;

constexpr float CAMERA_DISTANCE = 70;
constexpr float CAMERA_ELEVATION = 30;
constexpr float CAMERA_AZIMUTH = 180;

constexpr bool PALINDROME = true;

static const glm::vec4 capColor = { 1, 1, 1, 1 };
static const glm::vec4 wallColor = { 0.75f, 0.75f, 0.75f, 1 };
static const glm::vec4 strokeColor = { 0, 0, 0, 0.33f };

Episode1::Episode1(Sketch *sketch)
:
Episode(sketch),
surfaceClock(Clock::create())
{}

void Episode1::setup()
{
  loadStanza(InputSource::resource("stanza 1.xml"), 1);
  loadStanza(InputSource::resource("stanza 2.xml"), 2);
  loadStanza(InputSource::resource("stanza 3.xml"), 3);
  loadStanza(InputSource::resource("stanza 4.xml"), 4);
  loadStanza(InputSource::resource("stanza 5.xml"), 5);
  loadStanza(InputSource::resource("stanza 6.xml"), 6);

  font->setSize(FONT_SIZE);

  brickHeight = FONT_SIZE * BRICK_HEIGHT_RATIO;
  brickDepth = brickHeight * BRICK_DEPTH_RATIO;
  brickPadding = font->getCharAdvance(u' ') / 2;
  brickMargin = font->getCharAdvance(u' ') / 2;
  brickSpiral = SeaBrickSpiral(brickHeight, brickDepth, brickPadding, brickMargin);

  visualToSoundFactor = setupWords();
  spiralLength = setupBricks(0, words.back().endOffset);

  spiral.setup(surface, R1, R2, TURNS, DD1, DD2, spiralLength, brickHeight);

  // ---

  vector<Brick1>::const_iterator referenceBrick = findBrick(2, 1, 1); // EMPTRY BRICK BEFORE "Un"

  startOffset = 0;
  endOffset =  referenceBrick->startOffset - 44100 * 1.0f; // XXX

  // ---

  capBatch = IndexedVertexBatch<>(GL_TRIANGLES, brickSpiral.vertexBuffer, brickSpiral.capIndexBuffer);
  wallBatch = IndexedVertexBatch<>(GL_TRIANGLES, brickSpiral.vertexBuffer, brickSpiral.wallIndexBuffer);
  strokeBatch = IndexedVertexBatch<>(GL_LINES, brickSpiral.vertexBuffer, brickSpiral.strokeIndexBuffer);

  capBatch
    .setShader(colorShader)
    .setShaderColor(capColor);

  wallBatch
    .setShader(colorShader)
    .setShaderColor(wallColor);

  strokeBatch
    .setShader(colorShader)
    .setShaderColor(strokeColor);
}

void Episode1::update()
{
  unsigned int playingOffset = audioManager.getPlayingOffset();
  double playingRate = audioManager.getPlayingRate();

  if ((playingRate < 0) && (playingOffset <= startOffset))
  {
    if (PALINDROME)
    {
      audioManager.stop(); // XXX
      start(-playingRate);

      /*
       * TODO: FOLLOW-UP WITH THIS RECENTLY INTRODUCED FALL-BACK
       */
    }
    else
    {
      sketch->prevEpisode(playingRate);
      return;
    }
  }
  else if ((playingRate > 0) && (playingOffset >= endOffset))
  {
    sketch->nextEpisode(playingRate);
    return;
  }

  // ---

  for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
  {
    it->start = 0;
    it->end = 1;
  }

  vector<Brick1>::iterator brick = findBrick(playingOffset);

  if (brick != brickSpiral.bricks.end())
  {
    brick->start = (playingOffset - brick->startOffset) / float(brick->endOffset - brick->startOffset);

    for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brick; ++it)
    {
      it->end = 0;
    }

    // ---

    spiral.update(surface, surfaceClock->getTime(), SWELL_FACTOR);
    brickSpiral.update(spiral);

    // ---

    float position = brick->position + brick->start * brick->width;
    auto value = spiral.path.offsetToValue(position);

    Matrix matrix;
    value.applyToMatrix(matrix);

    cameraOrientation = matrix.getQuat();
    cameraTarget = value.position;
  }
}

void Episode1::draw()
{
  glClearColor(0.5f, 0.5f, 0.5f, 1);

  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  auto projectionMatrix = glm::perspective(45 * D2R, screenSize.x / (float)screenSize.y, 1.0f, 1000.0f);

  Matrix modelViewMatrix;
  modelViewMatrix
    .translate(0, 0, -CAMERA_DISTANCE)
    .rotateX(-CAMERA_ELEVATION * D2R)
    .rotateZ(-CAMERA_AZIMUTH * D2R)
    .rotateY(PI)
    .applyQuat<-1>(cameraOrientation)
    .translate(-cameraTarget);

  auto mvpMatrix = modelViewMatrix * projectionMatrix;

  State()
    .setShaderMatrix(mvpMatrix)
    .apply();

  //

  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);

  capBatch.flush();
  wallBatch.flush();

  glDepthMask(GL_FALSE);
  glDisable(GL_CULL_FACE);

  strokeBatch.flush();

  //

  font->setShader(textureAlphaShader);
  font->setColor(0, 0, 0, 0.75f);

  font->beginSequence(fontSequence);

  brickSpiral.drawText(*font, FONT_SIZE, POLYGON_OFFSET);

  font->endSequence();
  font->replaySequence(fontSequence);
}

void Episode1::start(double rate)
{
  if (!audioManager.isPlaying())
  {
    audioManager.start();
    audioManager.setPlayingRate(rate);

    if (rate < 0)
    {
      audioManager.setPlayingOffset(endOffset);
    }
    else
    {
      audioManager.setPlayingOffset(startOffset);
    }
  }

  // ---

  if (surfaceClock->getState() == Clock::STOPPED)
  {
    surfaceClock->start();
  }
}

void Episode1::stop(double rate)
{
  if (rate < 0)
  {
    audioManager.stop();
  }

  surfaceClock->stop();
}

void Episode1::pause()
{
  surfaceClock->stop();
}

void Episode1::resume()
{
  surfaceClock->start();
}

bool Episode1::isPlaying()
{
  return audioManager.isPlaying();
}

void Episode1::togglePlay()
{
  audioManager.togglePlay();
}

void Episode1::setPlayingRate(double rate)
{
  audioManager.setPlayingRate(rate);
}

float Episode1::setupBricks(float startOffset, float endOffset)
{
  font->setSize(FONT_SIZE);

  float position = 0;
  unsigned int lastOffset = startOffset;

  for (vector<Word>::const_iterator it = words.begin(); it != words.end(); ++it)
  {
    /*
     * EMPTY BRICK
     */
    if (it->startOffset != lastOffset)
    {
      float width = (it->startOffset - lastOffset) / visualToSoundFactor + brickMargin;
      brickSpiral.bricks.emplace_back(lastOffset, it->startOffset, position, width);
      position += width;
    }

    float width = font->getStringAdvance(it->text) + brickPadding * 2 + brickMargin;
    brickSpiral.bricks.emplace_back(position, width, it->factor, it);
    position += width;

    lastOffset = it->endOffset;
  }

  /*
   * EMPTY BRICK AT THE END
   */
  if (endOffset > lastOffset)
  {
    float width = (endOffset - lastOffset) / visualToSoundFactor + brickMargin;
    brickSpiral.bricks.emplace_back(lastOffset, endOffset, position, width);
    position += width;
  }

  // ---

  float l = TWO_PI * TURNS;
  float dr = (R2 - R1) / l;
  float DD = (DD2 - DD1) / l;

  for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
  {
    float r = sqrtf(R1 * R1 + 2 * dr * (it->position + brickMargin * 0.5f));
    float d = (r - R1) / dr;

    float segmentLength = DD1 + d * DD;
    it->fullSegmentCount = max<int>(1, (it->width - brickMargin) / segmentLength);
  }

  return position;
}

vector<Brick1>::iterator Episode1::findBrick(unsigned int offset)
{
  for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
  {
    if ((offset >= it->startOffset) && (offset < it->endOffset))
    {
      return it;
    }
  }

  return brickSpiral.bricks.end();
}

vector<Brick1>::iterator Episode1::findBrick(int stanzaIndex, int verseIndex, int wordIndex)
{
  for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
  {
    if (it->hasText && (it->stanzaIndex == stanzaIndex) && (it->verseIndex == verseIndex) && (it->wordIndex == wordIndex))
    {
      return it;
    }
  }

  return brickSpiral.bricks.end();
}


