#include "Episode3.h"
#include "Sketch.h"
#include "Camera.h"

using namespace std;
using namespace chr;

constexpr float AMPLITUDE = 24;
constexpr float PERIOD = 360; // DISTANCE BETWEEN CRESTS IN PIXELS
constexpr float VELOCITY = 4; // TIME BETWEEN CRESTS IN PIXELS/SECOND

constexpr float FONT_SIZE = 16;
constexpr float SEGMENT_LENGTH_RATIO = 0.67f; /* RELATIVE TO FONT-SIZE */
constexpr float BRICK_HEIGHT_RATIO = 1.5f; /* RELATIVE TO FONT-SIZE */
constexpr float BRICK_DEPTH_RATIO = 0.5f; /* RELATIVE TO BRICK HEIGHT */
constexpr float POLYGON_OFFSET = 1.0f;

constexpr int LINE_COUNT = 21;
constexpr float SHIFT_AMPLITUDE = 60;

const glm::vec4 fogColor =  { 0, 0, 0, 1 };

Episode3::Episode3(Sketch *sketch)
:
Episode(sketch),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag")),
surfaceClock(Clock::create())
{}

void Episode3::setup()
{
  loadStanza(InputSource::resource("stanza 4.xml"), 4, Stanza::FirstIndex(3, 3));
  loadStanza(InputSource::resource("stanza 5.xml"), 5);
  loadStanza(InputSource::resource("stanza 6.xml"), 6);

  font->setSize(FONT_SIZE);

  segmentLength = FONT_SIZE * SEGMENT_LENGTH_RATIO;
  brickHeight = FONT_SIZE * BRICK_HEIGHT_RATIO;
  brickDepth = brickHeight * BRICK_DEPTH_RATIO;
  brickPadding = font->getCharAdvance(u' ') / 2;
  brickMargin = font->getCharAdvance(u' ') / 2;

  /*
   * XXX: VALUES ARE RELATED TO TEXT AND CURRENT PARAMETERS (E.G. FONT-SIZE)
   */
  cornerPosition = -500;
  thinFlatBricksFarClip = -700;
  waveBricksFarClip = 500;

  // ---

  for (int i = 0; i < LINE_COUNT; i++)
  {
    waveBrickLines.emplace_back(brickHeight, brickDepth, brickPadding, brickMargin);
    thinWaveBrickLines.emplace_back(brickHeight, brickPadding, brickMargin);
    thinFlatBrickLines.emplace_back(brickHeight, brickPadding, brickMargin);
  }

  visualToSoundFactor = setupWords();
  lineLength = setupBricks(words.front().startOffset, words.back().endOffset);

  wave = new WaveSurface(AMPLITUDE, PERIOD, VELOCITY);
  setupLines(brickHeight);

  // ---

  referenceBrickLine = &waveBrickLines[LINE_COUNT >> 1];
  referenceBrick = findBrick(5, 1, 1) - 1; // EMPTRY BRICK BEFORE "Au"

  startOffset = referenceBrick->startOffset + (referenceBrick->endOffset - referenceBrick->startOffset) / 2; // MIDDLE OF THE EMPTY BRICK
  endOffset = words.back().endOffset + 44100 * 1.75f; // XXX: ADDING SOME EXTRA DELAY

  // ---

  waveBrickCapBatch = IndexedVertexBatch<>(GL_TRIANGLES, waveBrickVertexBuffer, waveBrickCapIndexBuffer);
  waveBrickWallBatch = IndexedVertexBatch<>(GL_TRIANGLES, waveBrickVertexBuffer, waveBrickWallIndexBuffer);
  waveBrickStrokeBatch = IndexedVertexBatch<>(GL_LINES, waveBrickVertexBuffer, waveBrickStrokeIndexBuffer);

  waveBrickCapBatch
    .setShader(fogColorShader)
    .setShaderColor(1, 1, 1, 1);

  waveBrickWallBatch
    .setShader(fogColorShader)
    .setShaderColor(0.80f, 0.80f, 0.80f, 1);

  waveBrickStrokeBatch
    .setShader(fogColorShader)
    .setShaderColor(0, 0, 0, 0.33f);

  //

  thinWaveBrickStrokeBatch = IndexedVertexBatch<>(GL_LINES, thinWaveBrickVertexBuffer, thinWaveBrickStrokeIndexBuffer);

  thinWaveBrickStrokeBatch
    .setShader(fogColorShader)
    .setShaderColor(1, 1, 1, 0.33f);

  //

  thinFlatBrickStrokeBatch = IndexedVertexBatch<>(GL_LINES, thinFlatBrickVertexBuffer, thinFlatBrickStrokeIndexBuffer);

  thinFlatBrickStrokeBatch
    .setShader(fogColorShader)
    .setShaderColor(1, 1, 1, 0.33f);
}

void Episode3::shutdown()
{
  delete wave;

  for (vector<WaveSurfaceLine*>::const_iterator it = waveLines.begin(); it != waveLines.end(); ++it)
  {
    delete *it;
  }
}

void Episode3::update()
{
  unsigned int playingOffset = audioManager.getPlayingOffset();
  double playingRate = audioManager.getPlayingRate();

  if ((playingRate < 0) && (playingOffset <= startOffset))
  {
    sketch->prevEpisode(playingRate);
    return;
  }
  else if ((playingRate > 0) && (playingOffset >= endOffset))
  {
    sketch->nextEpisode(playingRate);
    return;
  }

  vector<Brick3>::iterator brick = findBrick(playingOffset);

  if (brick != referenceBrickLine->bricks.end())
  {
    float u = (playingOffset - brick->startOffset) / (float)(brick->endOffset - brick->startOffset);
    scrollingOffset = referenceBrick->position - (brick->position + u * brick->width);
    scrollingExtra = false;

    for (vector<WaveBrickLine>::iterator it1 = waveBrickLines.begin(); it1 != waveBrickLines.end(); ++it1)
    {
      for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        if (it2->startOffset == brick->startOffset)
        {
          it2->start = u;
          it2->end = 1;
          it2->visible = true;
        }
        else if (it2->startOffset < brick->startOffset)
        {
          it2->start = 0;
          it2->end = 0;
          it2->visible = false;
        }
        else
        {
          it2->start = 0;
          it2->end = 1;
          it2->visible = true;
        }
      }
    }

    for (vector<ThinWaveBrickLine>::iterator it1 = thinWaveBrickLines.begin(); it1 != thinWaveBrickLines.end(); ++it1)
    {
      for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        if (it2->startOffset == brick->startOffset)
        {
          it2->start = 0;
          it2->end = u;
          it2->visible = true;
        }
        else if (it2->startOffset < brick->startOffset)
        {
          it2->start = 0;
          it2->end = 1;
          it2->visible = true;
        }
        else
        {
          it2->start = 0;
          it2->end = 0;
          it2->visible = false;
        }
      }
    }

    /*
     * WAVE BRICK-LINE CLIPPING
     */

    for (vector<WaveBrickLine>::iterator it1 = waveBrickLines.begin(); it1 != waveBrickLines.end(); ++it1)
    {
      for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        float positionStart = scrollingOffset + it1->shift + it2->position + brickMargin * 0.5f;

        if (positionStart > waveBricksFarClip)
        {
          it2->visible = false;
        }
      }
    }
  }
  else
  {
    for (vector<ThinWaveBrickLine>::iterator it1 = thinWaveBrickLines.begin(); it1 != thinWaveBrickLines.end(); ++it1)
    {
      for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        it2->start = 0;
        it2->end = 1;
        it2->visible = true;
      }
    }

    std::vector<Brick3>::const_iterator lastBrick = referenceBrickLine->bricks.end() - 1;
    float lastScrollingOffset = referenceBrick->position - (lastBrick->position + lastBrick->width);
    scrollingOffset = lastScrollingOffset - (playingOffset - lastBrick->endOffset) / visualToSoundFactor;
    scrollingExtra = true;
  }

  /*
   * THIN-WAVE BRICK-LINE CLIPPING
   */

  for (vector<ThinWaveBrickLine>::iterator it1 = thinWaveBrickLines.begin(); it1 != thinWaveBrickLines.end(); ++it1)
  {
    for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      float positionStart = scrollingOffset + it1->shift + it2->position + brickMargin * 0.5f;
      float positionEnd = positionStart + it2->width - brickMargin;

      if (positionEnd <= 0)
      {
        it2->visible = false;
      }
      else if ((0 >= positionStart) && (0 < positionEnd))
      {
        it2->start = (0 - positionStart) / (positionEnd - positionStart);;
        it2->end = 1;
      }
    }
  }

  /*
   * THIN-FLAT BRICK-LINE CLIPPING
   */

  for (vector<ThinFlatBrickLine>::iterator it1 = thinFlatBrickLines.begin(); it1 != thinFlatBrickLines.end(); ++it1)
  {
    for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      float positionStart = scrollingOffset + it1->shift + it2->position + brickMargin * 0.5f;
      float positionEnd = positionStart + it2->width - brickMargin;

      if ((positionStart >= 0) || (positionEnd < thinFlatBricksFarClip))
      {
        it2->visible = false;
      }
      else if ((0 >= positionStart) && (0 < positionEnd))
      {
        it2->start = 0;
        it2->end = (0 - positionStart) / (positionEnd - positionStart);
        it2->visible = true;
      }
      else
      {
        it2->start = 0;
        it2->end = 1;
        it2->visible = true;
      }
    }
  }

  updateBricks();
}

void Episode3::draw()
{
  glClearColor(0, 0, 0, 1);

  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  Camera camera;
  camera.setEyePoint(glm::vec3(175.47, 120.653, -261.614));
  camera.setOrientation(glm::vec3(0.999786, 0.00521308, 0.0199984), 3.65149);
  camera.setPerspective(45 * D2R, screenSize.x / (float)screenSize.y, 1.0f, 3000.0f);

  auto modelViewMatrix = camera.getModelViewMatrix();
  auto mvpMatrix = modelViewMatrix * camera.getProjectionMatrix();

  State state;
  state
    .setShaderMatrix(mvpMatrix)
    .setShaderUniform("u_fogSelector", 2) // EXP2
    .setShaderUniform("u_fogDensity", 0.002f)
    .setShaderUniform("u_fogColor", fogColor)
    .apply();

  //

  if (!scrollingExtra)
  {
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1, 1);

    waveBrickCapBatch.flush();
    waveBrickWallBatch.flush();

    glDepthMask(GL_FALSE);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_CULL_FACE);

    waveBrickStrokeBatch.flush();
  }

  thinWaveBrickStrokeBatch.flush();

  //

  font->setShader(fogTextureAlphaShader);

  if (!scrollingExtra)
  {
    font->setColor(0, 0, 0, 0.75f);
    font->beginSequence(waveFontSequence);

    for (vector<WaveBrickLine>::iterator it = waveBrickLines.begin(); it != waveBrickLines.end(); ++it)
    {
      it->drawText(*font, FONT_SIZE, POLYGON_OFFSET);
    }

    font->endSequence();
    font->replaySequence(waveFontSequence);
  }

  font->setColor(1, 1, 1, 0.5f);
  font->beginSequence(thinWaveFontSequence);

  for (vector<ThinWaveBrickLine>::iterator it = thinWaveBrickLines.begin(); it != thinWaveBrickLines.end(); ++it)
  {
    it->drawText(*font, FONT_SIZE, 0);
  }

  font->endSequence();
  font->replaySequence(thinWaveFontSequence);

  //

  modelViewMatrix.rotateY(HALF_PI);
  state
    .setShaderMatrix(modelViewMatrix * camera.getProjectionMatrix())
    .apply();

  thinFlatBrickStrokeBatch.flush();

  //

  font->setColor(1, 1, 1, 0.5f);
  font->beginSequence(thinFlatFontSequence);

  for (vector<ThinFlatBrickLine>::iterator it = thinFlatBrickLines.begin(); it != thinFlatBrickLines.end(); ++it)
  {
    it->drawText(*font, FONT_SIZE);
  }

  font->endSequence();
  font->replaySequence(thinFlatFontSequence);
}

void Episode3::start(double rate)
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

void Episode3::stop(double rate)
{
  if (rate > 0)
  {
    audioManager.stop();
  }

  surfaceClock->stop();
}

void Episode3::pause()
{
  surfaceClock->stop();
}

void Episode3::resume()
{
  surfaceClock->start();
}

bool Episode3::isPlaying()
{
  return audioManager.isPlaying();
}

void Episode3::togglePlay()
{
  audioManager.togglePlay();
}

void Episode3::setPlayingRate(double rate)
{
  audioManager.setPlayingRate(rate);
}

void Episode3::setupLines(float sampleSize)
{
  float y0 = LINE_COUNT * (brickHeight + brickMargin) * 0.5f;

  for (int i = 0; i < LINE_COUNT; i++)
  {
    float y = y0 - i * (brickHeight + brickMargin);
    float shift = SHIFT_AMPLITUDE * sinf(y * TWO_PI / PERIOD);

    WaveSurfaceLine *waveLine = new WaveSurfaceLine(wave, y, lineLength, segmentLength, sampleSize);
    waveLines.push_back(waveLine);

    waveBrickLines[i].waveLine = waveLine;
    waveBrickLines[i].lineLength = lineLength;
    waveBrickLines[i].segmentLength = segmentLength;
    waveBrickLines[i].shift = shift;

    thinWaveBrickLines[i].waveLine = waveLine;
    thinWaveBrickLines[i].lineLength = lineLength;
    thinWaveBrickLines[i].segmentLength = segmentLength;
    thinWaveBrickLines[i].shift = shift;

    thinFlatBrickLines[i].y = y;
    thinFlatBrickLines[i].shift = shift;
  }
}

float Episode3::setupBricks(float startOffset, float endOffset)
{
  font->setSize(FONT_SIZE);

  float position = cornerPosition;
  unsigned int lastOffset = startOffset;

  for (vector<Word>::const_iterator it1 = words.begin(); it1 != words.end(); ++it1)
  {
    /*
     * EMPTY BRICK
     */
    if (it1->startOffset != lastOffset)
    {
      float width = (it1->startOffset - lastOffset) / visualToSoundFactor + brickMargin;

      for (vector<WaveBrickLine>::iterator it2 = waveBrickLines.begin(); it2 != waveBrickLines.end(); ++it2)
      {
        it2->bricks.emplace_back(lastOffset, it1->startOffset, position, width);
      }
      for (vector<ThinWaveBrickLine>::iterator it2 = thinWaveBrickLines.begin(); it2 != thinWaveBrickLines.end(); ++it2)
      {
        it2->bricks.emplace_back(lastOffset, it1->startOffset, position, width);
      }
      for (vector<ThinFlatBrickLine>::iterator it2 = thinFlatBrickLines.begin(); it2 != thinFlatBrickLines.end(); ++it2)
      {
        it2->bricks.emplace_back(lastOffset, it1->startOffset, position, width);
      }
      position += width;
    }

    float width = font->getStringAdvance(it1->text) + brickPadding * 2 + brickMargin;

    for (vector<WaveBrickLine>::iterator it2 = waveBrickLines.begin(); it2 != waveBrickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(position, width, it1->factor, it1);
    }
    for (vector<ThinWaveBrickLine>::iterator it2 = thinWaveBrickLines.begin(); it2 != thinWaveBrickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(position, width, it1->factor, it1);
    }
    for (vector<ThinFlatBrickLine>::iterator it2 = thinFlatBrickLines.begin(); it2 != thinFlatBrickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(position, width, it1->factor, it1);
    }
    position += width;

    lastOffset = it1->endOffset;
  }

  /*
   * EMPTY BRICK AT THE END
   */
  if (endOffset > lastOffset)
  {
    float width = (endOffset - lastOffset) / visualToSoundFactor + brickMargin;

    for (vector<WaveBrickLine>::iterator it2 = waveBrickLines.begin(); it2 != waveBrickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(lastOffset, endOffset, position, width);
    }
    for (vector<ThinWaveBrickLine>::iterator it2 = thinWaveBrickLines.begin(); it2 != thinWaveBrickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(lastOffset, endOffset, position, width);
    }
    for (vector<ThinFlatBrickLine>::iterator it2 = thinFlatBrickLines.begin(); it2 != thinFlatBrickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(lastOffset, endOffset, position, width);
    }
    position += width;
  }

  // ---

  for (vector<WaveBrickLine>::iterator it1 = waveBrickLines.begin(); it1 != waveBrickLines.end(); ++it1)
  {
    for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      it2->fullSegmentCount = max<int>(1, (it2->width - brickMargin) / segmentLength);
    }
  }

  for (vector<ThinWaveBrickLine>::iterator it1 = thinWaveBrickLines.begin(); it1 != thinWaveBrickLines.end(); ++it1)
  {
    for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      it2->fullSegmentCount = max<int>(1, (it2->width - brickMargin) / segmentLength);
    }
  }

  for (vector<ThinFlatBrickLine>::iterator it1 = thinFlatBrickLines.begin(); it1 != thinFlatBrickLines.end(); ++it1)
  {
    for (vector<Brick3>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      it2->fullSegmentCount = max<int>(1, (it2->width - brickMargin) / segmentLength);
    }
  }

  return position;
}

void Episode3::updateBricks()
{
  for (vector<WaveSurfaceLine*>::iterator it = waveLines.begin(); it != waveLines.end(); ++it)
  {
    (*it)->compute(surfaceClock->getTime());
  }

  // ---

  auto &waveBrickVertices = waveBrickVertexBuffer->storage;
  waveBrickVertices.clear();
  waveBrickVertexBuffer.requestUpload();

  auto &waveBrickCapIndices = waveBrickCapIndexBuffer->storage;
  waveBrickCapIndices.clear();
  waveBrickCapIndexBuffer.requestUpload();

  auto &waveBrickWallIndices = waveBrickWallIndexBuffer->storage;
  waveBrickWallIndices.clear();
  waveBrickWallIndexBuffer.requestUpload();

  auto &waveBrickStrokeIndices = waveBrickStrokeIndexBuffer->storage;
  waveBrickStrokeIndices.clear();
  waveBrickStrokeIndexBuffer.requestUpload();

  for (vector<WaveBrickLine>::iterator it = waveBrickLines.begin(); it != waveBrickLines.end(); ++it)
  {
    it->updateVertices(waveBrickVertices, scrollingOffset);
    it->updateIndices(waveBrickCapIndices, waveBrickWallIndices, waveBrickStrokeIndices);
  }

  // ---

  auto &thinWaveBrickVertices = thinWaveBrickVertexBuffer->storage;
  thinWaveBrickVertices.clear();
  thinWaveBrickVertexBuffer.requestUpload();

  auto &thinWaveBrickStrokeIndices = thinWaveBrickStrokeIndexBuffer->storage;
  thinWaveBrickStrokeIndices.clear();
  thinWaveBrickStrokeIndexBuffer.requestUpload();

  for (vector<ThinWaveBrickLine>::iterator it = thinWaveBrickLines.begin(); it != thinWaveBrickLines.end(); ++it)
  {
    it->updateVertices(thinWaveBrickVertices, scrollingOffset);
    it->updateIndices(thinWaveBrickStrokeIndices);
  }

  // ---

  auto &thinFlatBrickVertices = thinFlatBrickVertexBuffer->storage;
  thinFlatBrickVertices.clear();
  thinFlatBrickVertexBuffer.requestUpload();

  auto &thinFlatBrickStrokeIndices = thinFlatBrickStrokeIndexBuffer->storage;
  thinFlatBrickStrokeIndices.clear();
  thinFlatBrickStrokeIndexBuffer.requestUpload();

  for (vector<ThinFlatBrickLine>::iterator it = thinFlatBrickLines.begin(); it != thinFlatBrickLines.end(); ++it)
  {
    it->updateVertices(thinFlatBrickVertices, scrollingOffset);
    it->updateIndices(thinFlatBrickStrokeIndices);
  }
}

vector<Brick3>::iterator Episode3::findBrick(unsigned int offset)
{
  for (vector<Brick3>::iterator it = referenceBrickLine->bricks.begin(); it != referenceBrickLine->bricks.end(); ++it)
  {
    if ((offset >= it->startOffset) && (offset < it->endOffset))
    {
      return it;
    }
  }

  return referenceBrickLine->bricks.end();
}

vector<Brick3>::const_iterator Episode3::findBrick(int stanzaIndex, int verseIndex, int wordIndex)
{
  for (vector<Brick3>::iterator it = referenceBrickLine->bricks.begin(); it != referenceBrickLine->bricks.end(); ++it)
  {
    if (it->hasText && (it->stanzaIndex == stanzaIndex) && (it->verseIndex == verseIndex) && (it->wordIndex == wordIndex))
    {
      return it;
    }
  }

  return referenceBrickLine->bricks.end();
}
