#include "Episode2.h"
#include "Sketch.h"
#include "Camera.h"

using namespace std;
using namespace chr;

constexpr float FONT_SIZE = 24;
constexpr float BRICK_HEIGHT_RATIO = 1.5f; /* RELATIVE TO FONT-SIZE */
constexpr float BRICK_DEPTH_RATIO = 0.5f; /* RELATIVE TO BRICK HEIGHT */

constexpr int LINE_COUNT = 18;

static const glm::vec4 fogColor = { 0, 0, 0, 1 };
static const glm::vec4 capColor = { 0.9f, 0.9f, 0.9f, 1 };
static const glm::vec4 wallColor = { 1, 1, 1, 1 };
static const glm::vec4 strokeColor = { 0, 0, 0, 0.2f };

Episode2::Episode2(Sketch *sketch)
:
Episode(sketch),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag"))
{}

void Episode2::setup()
{
  loadStanza(InputSource::resource("stanza 2.xml"), 2);
  loadStanza(InputSource::resource("stanza 3.xml"), 3);
  loadStanza(InputSource::resource("stanza 4.xml"), 4);
  loadStanza(InputSource::resource("stanza 5.xml"), 5, Stanza::FirstIndex(), Stanza::LastIndex(3, 4));

  font->setSize(FONT_SIZE);

  brickHeight = FONT_SIZE * BRICK_HEIGHT_RATIO;
  brickDepth = brickHeight * BRICK_DEPTH_RATIO;
  brickPadding = font->getCharAdvance(u' ') / 3;
  brickMargin = font->getCharAdvance(u' ') / 6;

  /*
   * XXX: VALUES ARE RELATED TO TEXT AND CURRENT PARAMETERS (E.G. FONT-SIZE)
   */
  nearClip = 100;
  farClip = 1300;

  // ---

  for (int i = 0; i < LINE_COUNT; i++)
  {
    brickLines.emplace_back(brickHeight, brickDepth, brickPadding);
  }

  visualToSoundFactor = setupWords();
  setupBricks(words.front().startOffset - 44100 * 10.0f, words.back().endOffset); // XXX

  setupLines();

  // ----

  referenceBrickLine = &brickLines[0];
  referenceBrick = findBrick(5, 1, 1) - 1; // EMPTRY BRICK BEFORE "Au"

  startOffset = words.front().startOffset - 44100 * 1.0f; // XXX
  endOffset = referenceBrick->startOffset + (referenceBrick->endOffset - referenceBrick->startOffset) / 2; // MIDDLE OF THE EMPTY BRICK

  // ---

  capBatch = IndexedVertexBatch<>(GL_TRIANGLES, vertexBuffer, capIndexBuffer);
  wallBatch = IndexedVertexBatch<>(GL_TRIANGLES, vertexBuffer, wallIndexBuffer);
  strokeBatch = IndexedVertexBatch<>(GL_LINES, vertexBuffer, strokeIndexBuffer);

  capBatch
    .setShader(fogColorShader)
    .setShaderColor(capColor);

  wallBatch
    .setShader(fogColorShader)
    .setShaderColor(wallColor);

  strokeBatch
    .setShader(fogColorShader)
    .setShaderColor(strokeColor);
}

void Episode2::update()
{
  unsigned int playingOffset = audioManager.getPlayingOffset();
  double playingRate = audioManager.getPlayingRate();

  scrollingOffset = (playingOffset - startOffset) / visualToSoundFactor;

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

  for (vector<BrickLine>::iterator it1 = brickLines.begin(); it1 != brickLines.end(); ++it1)
  {
    for (vector<Brick2>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      it2->v = 0;
    }

    it1->shift = 0;
  }

  vector<Brick2>::iterator brick = findBrick(playingOffset);

  if (brick != referenceBrickLine->bricks.end())
  {
    if (brick->hasText)
    {
      float v = (playingOffset - brick->startOffset) / float(brick->endOffset - brick->startOffset);
      float b = v * (brick->textWidth + brickPadding * 2 + brick->marginLeft + brick->marginRight) + (1 - v) * brick->slotWidth;
      float shift = brick->slotWidth - b;

      for (vector<BrickLine>::iterator it1 = brickLines.begin(); it1 != brickLines.end(); ++it1)
      {
        for (vector<Brick2>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
        {
          if (it2->startOffset == brick->startOffset)
          {
            it2->v = v;
            it1->shift = shift;
          }
        }
      }
    }

    for (vector<BrickLine>::iterator it1 = brickLines.begin(); it1 != brickLines.end(); ++it1)
    {
      for (vector<Brick2>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        if (it2->hasText)
        {
          if (it2->startOffset < brick->startOffset)
          {
            it2->v = 1;
            it1->shift += it2->slotWidth - (it2->textWidth + brickPadding * 2 + it2->marginLeft + it2->marginRight);
          }
        }
      }
    }

    /*
     * CALCULATING THE POSITION OF THE BRICKS, REGARDLESS OF THEIR VISIBILITY
     */

    for (vector<BrickLine>::iterator it1 = brickLines.begin(); it1 != brickLines.end(); ++it1)
    {
      float position = it1->shift;

      for (vector<Brick2>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        it2->position = position;

        if (it2->hasText)
        {
          position += (it2->v * (it2->textWidth + brickPadding * 2 + it2->marginLeft + it2->marginRight) + (1 - it2->v) * it2->slotWidth);
        }
        else
        {
          position += it2->slotWidth;
        }
      }
    }

    /*
     * CLIPPING THE BRICKS
     */

    for (vector<BrickLine>::iterator it1 = brickLines.begin(); it1 != brickLines.end(); ++it1)
    {
      for (vector<Brick2>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
      {
        float positionStart = it2->position - scrollingOffset;
        float positionEnd = positionStart + (it2->hasText ? (it2->v * (it2->textWidth + brickPadding * 2 + it2->marginLeft + it2->marginRight) + (1 - it2->v) * it2->slotWidth) : it2->slotWidth);

        if ((positionEnd < nearClip) || (positionStart > farClip))
        {
          it2->visible = false;
        }
        else
        {
          it2->visible = true;
        }
      }
    }
  }

  updateBricks();
}

void Episode2::draw()
{
  glClearColor(0, 0, 0, 1);

  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  Camera camera;
  camera.setEyePoint(glm::vec3(-123.619, -296, -413.715));
  camera.setOrientation(glm::vec3(0.982254, 0.0372618, -0.183819), 2.74851);
  camera.setPerspective(45 * D2R, screenSize.x / (float)screenSize.y, 10.0f, 1000.0f);

  auto modelViewMatrix = camera.getModelViewMatrix();
  modelViewMatrix.translate(-scrollingOffset - (44100 * 6.5f / visualToSoundFactor), 0, 0); // XXX

  auto mvpMatrix = modelViewMatrix * camera.getProjectionMatrix();

  State()
    .setShaderMatrix(mvpMatrix)
    .setShaderUniform("u_fogSelector", 2) // EXP2
    .setShaderUniform("u_fogDensity", 0.00175f)
    .setShaderUniform("u_fogColor", fogColor)
    .apply();

  //

  glDepthMask(GL_TRUE);
  glEnable(GL_CULL_FACE);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  capBatch.flush();
  wallBatch.flush();

  glDepthMask(GL_FALSE);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_CULL_FACE);

  strokeBatch.flush();

  //

  font->setShader(fogTextureAlphaShader);
  font->setColor(0, 0, 0, 0.75f);

  font->beginSequence(fontSequence);

  for (vector<BrickLine>::iterator it = brickLines.begin(); it != brickLines.end(); ++it)
  {
    it->drawText(*font, FONT_SIZE);
  }

  font->endSequence();
  font->replaySequence(fontSequence);
}

void Episode2::start(double rate)
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
}

bool Episode2::isPlaying()
{
  return audioManager.isPlaying();
}

void Episode2::togglePlay()
{
  audioManager.togglePlay();
}

void Episode2::setPlayingRate(double rate)
{
  audioManager.setPlayingRate(rate);
}

void Episode2::setupLines()
{
  brickLines[1].ox = -(44100 * 2.0f) / visualToSoundFactor;
  brickLines[2].ox = -(44100 * 1.0f) / visualToSoundFactor;
  brickLines[3].ox = -(44100 * 1.5f) / visualToSoundFactor;
  brickLines[4].ox = -(44100 * 2.5f) / visualToSoundFactor;
  brickLines[5].ox = -(44100 * 0.5f) / visualToSoundFactor;
  brickLines[6].ox = -(44100 * 2.0f) / visualToSoundFactor;
  brickLines[7].ox = -(44100 * 2.5f) / visualToSoundFactor;
  brickLines[0].ox = 0;
  brickLines[8].ox = -(44100 * 2.0f) / visualToSoundFactor;
  brickLines[9].ox = -(44100 * 1.0f) / visualToSoundFactor;
  brickLines[10].ox = -(44100 * 1.5f) / visualToSoundFactor;
  brickLines[11].ox = -(44100 * 2.5f) / visualToSoundFactor;
  brickLines[12].ox = -(44100 * 0.5f) / visualToSoundFactor;
  brickLines[13].ox = -(44100 * 2.0f) / visualToSoundFactor;
  brickLines[14].ox = -(44100 * 2.5f) / visualToSoundFactor;
  brickLines[15].ox = -(44100 * 1.0f) / visualToSoundFactor;
  brickLines[16].ox = -(44100 * 0.0f) / visualToSoundFactor;
  brickLines[17].ox = -(44100 * 0.5f) / visualToSoundFactor;

  brickLines[1].oy = (brickHeight + brickMargin) * -1;
  brickLines[2].oy = (brickHeight + brickMargin) * -2;
  brickLines[3].oy = (brickHeight + brickMargin) * -3;
  brickLines[4].oy = (brickHeight + brickMargin) * -4;
  brickLines[5].oy = (brickHeight + brickMargin) * -5;
  brickLines[6].oy = (brickHeight + brickMargin) * -6;
  brickLines[7].oy = (brickHeight + brickMargin) * -7;
  brickLines[0].oy = 0;
  brickLines[8].oy = (brickHeight + brickMargin) * +1;
  brickLines[9].oy = (brickHeight + brickMargin) * +2;
  brickLines[10].oy = (brickHeight + brickMargin) * +3;
  brickLines[11].oy = (brickHeight + brickMargin) * +4;
  brickLines[12].oy = (brickHeight + brickMargin) * +5;
  brickLines[13].oy = (brickHeight + brickMargin) * +6;
  brickLines[14].oy = (brickHeight + brickMargin) * +7;
  brickLines[15].oy = (brickHeight + brickMargin) * +8;
  brickLines[16].oy = (brickHeight + brickMargin) * +9;
  brickLines[17].oy = (brickHeight + brickMargin) * +10;
}

void Episode2::setupBricks(float startOffset, float endOffset)
{
  font->setSize(FONT_SIZE);

  unsigned int lastOffset = startOffset;

  for (vector<Word>::const_iterator it1 = words.begin(); it1 != words.end(); ++it1)
  {
    /*
     * EMPTY BRICK
     */
    if (it1->startOffset != lastOffset)
    {
      float width = (it1->startOffset - lastOffset) / visualToSoundFactor;

      for (vector<BrickLine>::iterator it2 = brickLines.begin(); it2 != brickLines.end(); ++it2)
      {
        it2->bricks.emplace_back(lastOffset, it1->startOffset, width);
      }
    }

    float textWidth = font->getStringAdvance(it1->text);
    float slotWidth = it1->factor * textWidth;

    for (vector<BrickLine>::iterator it2 = brickLines.begin(); it2 != brickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(slotWidth, textWidth, it1);
    }

    lastOffset = it1->endOffset;
  }

  /*
   * EMPTY BRICK AT THE END
   */
  if (endOffset > lastOffset)
  {
    float width = (endOffset - lastOffset) / visualToSoundFactor;

    for (vector<BrickLine>::iterator it2 = brickLines.begin(); it2 != brickLines.end(); ++it2)
    {
      it2->bricks.emplace_back(lastOffset, endOffset, width);
    }
  }

  // ---

  for (vector<BrickLine>::iterator it1 = brickLines.begin(); it1 != brickLines.end(); ++it1)
  {
    for (vector<Brick2>::iterator it2 = it1->bricks.begin(); it2 != it1->bricks.end(); ++it2)
    {
      if (it2->hasText)
      {
        it2->marginLeft = brickMargin;
        it2->marginRight = ((it2 + 1 != it1->bricks.end()) && !((it2 + 1)->hasText)) ? brickMargin : 0;
      }
    }
  }
}

void Episode2::updateBricks()
{
  auto &vertices = vertexBuffer->storage;
  vertices.clear();
  vertexBuffer.requestUpload();

  auto &capIndices = capIndexBuffer->storage;
  capIndices.clear();
  capIndexBuffer.requestUpload();

  auto &wallIndices = wallIndexBuffer->storage;
  wallIndices.clear();
  wallIndexBuffer.requestUpload();

  auto &strokeIndices = strokeIndexBuffer->storage;
  strokeIndices.clear();
  strokeIndexBuffer.requestUpload();

  for (vector<BrickLine>::iterator it = brickLines.begin(); it != brickLines.end(); ++it)
  {
    it->updateVertices(vertices);
    it->updateIndices(capIndices, wallIndices, strokeIndices);
  }
}

vector<Brick2>::iterator Episode2::findBrick(unsigned int offset)
{
  for (vector<Brick2>::iterator it = referenceBrickLine->bricks.begin(); it != referenceBrickLine->bricks.end(); ++it)
  {
    if ((offset >= it->startOffset) && (offset < it->endOffset))
    {
      return it;
    }
  }

  return referenceBrickLine->bricks.end();
}

vector<Brick2>::const_iterator Episode2::findBrick(int stanzaIndex, int verseIndex, int wordIndex)
{
  for (vector<Brick2>::iterator it = referenceBrickLine->bricks.begin(); it != referenceBrickLine->bricks.end(); ++it)
  {
    if (it->hasText && (it->stanzaIndex == stanzaIndex) && (it->verseIndex == verseIndex) && (it->wordIndex == wordIndex))
    {
      return it;
    }
  }

  return referenceBrickLine->bricks.end();
}
