#include "Episode4.h"
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
constexpr float POLYGON_OFFSET = 0.275f;

constexpr float CAMERA_DISTANCE = 167;
constexpr float CAMERA_ELEVATION = 60;
constexpr float CAMERA_AZIMUTH = 180;

constexpr bool PALINDROME = true;

Episode4::Episode4(Sketch *sketch)
:
Episode(sketch),
clock(Clock::create()),
surfaceClock(Clock::create())
{}

void Episode4::setup()
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

  // ---

  brickSpiral = SeaBrickSpiral(brickHeight, brickDepth, brickPadding, brickMargin);

  visualToSoundFactor = setupWords();
  spiralLength = setupBricks(0, words.back().endOffset);

  spiral.setup(surface, R1, R2, TURNS, DD1, DD2, spiralLength, brickHeight);

  // ---

  randomizeRuns = true;
  rand.seed(123456789);

  // ---

  capBatch = IndexedVertexBatch<>(GL_TRIANGLES, brickSpiral.vertexBuffer, brickSpiral.capIndexBuffer);
  wallBatch = IndexedVertexBatch<>(GL_TRIANGLES, brickSpiral.vertexBuffer, brickSpiral.wallIndexBuffer);
  strokeBatch = IndexedVertexBatch<>(GL_LINES, brickSpiral.vertexBuffer, brickSpiral.strokeIndexBuffer);

  capBatch
    .setShader(colorShader)
    .setShaderColor(1, 1, 1, 1);

  wallBatch
    .setShader(colorShader)
    .setShaderColor(0.75f, 0.75f, 0.75f, 1);

  strokeBatch
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 0.33f);
}

void Episode4::update()
{
  if (playing)
  {
    double now = clock->getTime();

    if ((playingRate < 0) && (now <= startTime))
    {
      audioManager.stop(false); // XXX
      sketch->prevEpisode(playingRate);

      return;
    }
    else if ((playingRate > 0) && (now >= endTime))
    {
      if (PALINDROME)
      {
        for (vector<Run>::iterator it = runs.begin(); it != runs.end(); ++it)
        {
          if (it->channel)
          {
            audioManager.stopRun(it->channel);
            it->channel = nullptr;
            it->playing = false;
          }
        }

        start(-playingRate);

        /*
         * TODO: FOLLOW-UP WITH THIS RECENTLY INTRODUCED FALL-BACK
         */
      }
      else
      {
        audioManager.stop(false); // XXX
        sketch->nextEpisode(playingRate);

        return;
      }
    }

    for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
    {
      it->end = 0;
    }

    for (vector<Run>::iterator it1 = runs.begin(); it1 != runs.end(); ++it1)
    {
      if (it1->frequency < 0)
      {
        double offset = it1->endBrick->endOffset + it1->frequency * (now - it1->startTime) * 44100.0;

        if (((playingRate < 0) && (it1->endBrick->endOffset <= offset)) || ((playingRate > 0) && (it1->startBrick->startOffset >= offset)))
        {
          if (it1->playing)
          {
            audioManager.stopRun(it1->channel);
            it1->channel = nullptr;
            it1->playing = false;
          }

          for (vector<Brick1>::iterator it2 = it1->startBrick; it2 <= it1->endBrick; ++it2)
          {
            if (playingRate < 0)
            {
              it2->end = 0;
            }
            else
            {
              it2->start = 0;
              it2->end = 1;
            }
          }
        }
        else
        {
          for (vector<Brick1>::iterator it2 = it1->startBrick; it2 <= it1->endBrick; ++it2)
          {
            if ((offset >= it2->startOffset) && (offset < it2->endOffset))
            {
              if (!it1->playing)
              {
                it1->channel = audioManager.playRun(offset, playingRate * it1->frequency);
                it1->playing = true;
              }

              it2->start = (offset - it2->startOffset) / (float)(it2->endOffset - it2->startOffset);
              it2->end = 1;
            }
            else if (it2->startOffset >= offset)
            {
              it2->start = 0;
              it2->end = 1;
            }
          }
        }
      }
      else
      {
        double offset = it1->startBrick->startOffset + it1->frequency * (now - it1->startTime) * 44100.0;

        if (((playingRate < 0) && (it1->startBrick->startOffset >= offset)) || ((playingRate > 0) && (it1->endBrick->endOffset <= offset)))
        {
          if (it1->playing)
          {
            audioManager.stopRun(it1->channel);
            it1->channel = nullptr;
            it1->playing = false;
          }

          for (vector<Brick1>::iterator it2 = it1->startBrick; it2 <= it1->endBrick; ++it2)
          {
            if (playingRate < 0)
            {
              it2->end = 0;
            }
            else
            {
              it2->start = 0;
              it2->end = 1;
            }
          }
        }
        else
        {
          for (vector<Brick1>::iterator it2 = it1->startBrick; it2 <= it1->endBrick; ++it2)
          {
            if ((offset >= it2->startOffset) && (offset < it2->endOffset))
            {
              if (!it1->playing)
              {
                it1->channel = audioManager.playRun(offset, playingRate * it1->frequency);
                it1->playing = true;
              }

              it2->start = 0;
              it2->end = (offset - it2->startOffset) / (float)(it2->endOffset - it2->startOffset);
            }
            else if (it2->endOffset <= offset)
            {
              it2->start = 0;
              it2->end = 1;
            }
          }
        }
      }
    }
  }

  updateBricks();
}

void Episode4::draw()
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
    .rotateY(PI);

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

void Episode4::start(double rate)
{
  if (randomizeRuns)
  {
    randomizeRuns = false;

    /*
     * ASSERTIONS:
     * - THE FIRST BRICK IS EMPTY
     * - THE LAST BRICK IS NOT EMPTY
     */

    runs.clear();

    vector<Brick1>::iterator firstBrick = findBrick(1, 1, 1) - 1;
    vector<Brick1>::iterator startBrick = brickSpiral.bricks.end();
    int verseIndex = 0;

    for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
    {
      if (it == firstBrick)
      {
        runs.emplace_back(it, it);
      }
      else if (it->hasText)
      {
        if (it->verseIndex != verseIndex)
        {
          vector<Brick1>::iterator endBrick = it - 1;

          if (startBrick != brickSpiral.bricks.end())
          {
            runs.emplace_back(startBrick, endBrick);
          }

          verseIndex = it->verseIndex;
          startBrick = it;
        }
      }
    }

    runs.emplace_back(startBrick, brickSpiral.bricks.end() - 1);

    // ---

    double t = 0.25; // XXX

    for (vector<Run>::iterator it = runs.begin(); it != runs.end(); ++it)
    {
      it->setProperties(rand.nextFloat(0.8f, 1.2f), t); // XXX
      t += it->getDuration() * rand.nextFloat(0.125f, 0.33f); // XXX
    }

    // ---

    startTime = 0;
    endTime = numeric_limits<double>::min();

    for (vector<Run>::iterator it = runs.begin(); it != runs.end(); ++it)
    {
      endTime = max<double>(endTime, it->endTime);
    }

    endTime += 0.25; // XXX
  }

  // ---

  clock->stop();

  if (rate < 0)
  {
    clock->setTime(endTime);
  }
  else
  {
    clock->setTime(startTime);
  }

  clock->setRate(rate);
  clock->start();

  playing = true;
  playingRate = rate;

  audioManager.start(false);

  // ---

  if (surfaceClock->getState() == Clock::STOPPED)
  {
    surfaceClock->start();
  }
}

void Episode4::stop(double rate)
{
  for (vector<Run>::iterator it = runs.begin(); it != runs.end(); ++it)
  {
    if (it->channel)
    {
      audioManager.stopRun(it->channel);
    }
  }

  audioManager.stop(false);

  surfaceClock->stop();
  randomizeRuns = true;
}

void Episode4::pause()
{
  surfaceClock->stop();
}

void Episode4::resume()
{
  surfaceClock->start();
}

bool Episode4::isPlaying()
{
  return playing;
}

void Episode4::togglePlay()
{
  for (vector<Run>::iterator it = runs.begin(); it != runs.end(); ++it)
  {
    if (it->playing)
    {
      audioManager.pauseRun(it->channel);
      it->playing = false;
    }
    else
    {
      audioManager.resumeRun(it->channel);
      it->playing = true;
    }
  }

  audioManager.togglePlay();

  if (playing)
  {
    clock->stop();
    playing = false;
  }
  else
  {
    clock->start();
    playing = true;
  }
}

void Episode4::setPlayingRate(double rate)
{
  for (vector<Run>::iterator it = runs.begin(); it != runs.end(); ++it)
  {
    if (it->playing)
    {
      audioManager.setRunRate(it->channel, rate * it->frequency);
    }
  }

  clock->stop();
  clock->setRate(rate);
  clock->start();

  playingRate = rate;
}

float Episode4::setupBricks(float startOffset, float endOffset)
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

  /*
   * DEFINING THE FULL-SEGMENT-LENGTHS
   */

  for (vector<Brick1>::iterator it = brickSpiral.bricks.begin(); it != brickSpiral.bricks.end(); ++it)
  {
    float r = sqrtf(R1 * R1 + 2 * dr * (it->position + brickMargin * 0.5f));
    float d = (r - R1) / dr;

    float segmentLength = DD1 + d * DD;
    it->fullSegmentCount = max<int>(1, (it->width - brickMargin) / segmentLength);
  }

  return position;
}

void Episode4::updateBricks()
{
  spiral.update(surface, surfaceClock->getTime(), SWELL_FACTOR);
  brickSpiral.update(spiral);
}

vector<Brick1>::iterator Episode4::findBrick(unsigned int offset)
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

vector<Brick1>::iterator Episode4::findBrick(int stanzaIndex, int verseIndex, int wordIndex)
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
