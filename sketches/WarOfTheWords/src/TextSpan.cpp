#include "TextSpan.h"

#include "WarTextBox.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

/*
 * THE SOLUTION TO LIMIT Z-FIGHTING BETWEEN ADJACENT SPANS
 */
static constexpr float OX = 0.1f;
static constexpr float OY = 0.3f;

TextSpan::TextSpan(const WarTextBox &parent, int id, int start, int end, int hitsPerChar)
:
id(id),
start(start),
end(end),
hitsPerChar(hitsPerChar)
{
  text = parent.text.substr(start, end - start);
  font = parent.font;
}

void TextSpan::reset()
{
  elevating = false;
  collapsing = false;
  rotating = false;
  launching = false;
  opened = false;
  burried = false;

  missile.reset();
}

void TextSpan::update(int line, float x1, float y1, float x2, float y2)
{
  this->line = line;

  this->x1 = x1 + OX;
  this->y1 = y1 + OY;
  this->x2 = x2 - OX;
  this->y2 = y2 - OY;
}

void TextSpan::update(vector<shared_ptr<TextSpan>> topSpans, vector<std::shared_ptr<TextSpan>> bottomSpans)
{
  this->topSpans = topSpans;
  this->bottomSpans = bottomSpans;
}

void TextSpan::update(size_t minLength, size_t maxLength)
{
  float length = (end - start);
  elevationDuration = length / (float) maxLength * elevationDurationMax;
  elevationMax = log10(length * 0.6f) * 40; // XXX: SO THAT SPANS WITH A LOT OF CHARACTERS WON'T BE TOO TALL...
  collapseAmount = elevationMax / length / hitsPerChar;
}

void TextSpan::update(float offsetX, float offsetY)
{
  textureOffsetX = offsetX;
  textureOffsetY = offsetY;
}

Rectf TextSpan::getBounds()
{
  return Rectf(x1, y1, x2 - x1, y2 - y1);
}

bool TextSpan::isLimited(std::vector<std::shared_ptr<TextSpan>> spans, float limit)
{
  for (auto span : spans)
  {
    if (span && (span->elevation > limit))
    {
      return true;
    }
  }

  return false;
}


void TextSpan::run(float elapsed)
{
  if (!burried)
  {
    float t = elapsed - t0;

    if (elevating)
    {
      if (t < elevationDuration)
      {
        elevation = fminf(1, t / elevationDuration) * elevationMax;
      }
      else
      {
        elevation = elevationMax;
        elevating = false;
      }

      return;
    }

    if (collapsing)
    {
      if (elevation <= 0)
      {
        burried = true;
        return;
      }

      if (elevation <= collapseElevationEnd)
      {
        collapsing = false;
      }
      else
      {
        elevation = collapseElevationStart - collapseAmount * (elapsed - collapseT0);
      }
    }

    if (launching && missile && (t > launchDuration))
    {
      missile->launch();
      missile.reset();
    }

    if (rotating)
    {
      if (t <= rotationDuration)
      {
        float limit = 0;
        float amplituteTop = isLimited(topSpans, limit) ? amplitudeLimited : amplitudeMax;
        float amplituteBottom = -(isLimited(bottomSpans, limit) ? amplitudeLimited : amplitudeMax);

        float factor1 = fminf(1, t / rotationDuration);
        float factor2 = (rotationDirection < 0 ? (1 - factor1) : factor1);

        angleTop = factor2 * amplituteTop;
        angleBottom = factor2 * amplituteBottom;
      }
      else
      {
        if (rotationDirection < 0)
        {
          opened = false;
          launching = false;
        }

        rotating = false;
        t0 = elapsed;
      }

      return;
    }

    if (opened && !rotating)
    {
      if (t > closingDuration)
      {
        rotating = true;
        rotationDirection = -1;
        t0 = elapsed;
      }
    }
  }}

// --- DRAWING ---

void TextSpan::draw(DrawingContext &context, int pass)
{
  if (!burried)
  {
    switch (pass)
    {
      case 0 :
        drawPass0(context);
        break;

      case 1 :
        drawPass1(context);
        break;

      case 2 :
        drawPass2(context);
        break;

      case 3 :
        drawPass3(context);
        break;

      case 4 :
        drawPass4(context);
        break;

      case 5 :
        drawPass5(context);
        break;
    }
  }
}

/*
 * ELEVATION (SIDES)
 */

void TextSpan::drawPass0(DrawingContext &context)
{
  if (elevation > 0)
  {
    glm::vec3 baseColor = PALETTE[id]; // TODO: USE SATURATED COLORS

    glm::vec4 color1(baseColor * 0.95f, 1);
    glm::vec4 color2(baseColor * 0.90f, 1);

    //

    context.flatBatch
      .addVertex(x1, y2, elevation, color1)
      .addVertex(x2, y2, elevation, color1)
      .addVertex(x2, y2, 0, color1)
      .addVertex(x1, y2, 0, color1);

    context.flatBatch
      .addIndices(0, 1, 2, 2, 3, 0)
      .incrementIndices(4);

    context.flatBatch
      .addVertex(x2, y1, elevation, color1)
      .addVertex(x1, y1, elevation, color1)
      .addVertex(x1, y1, 0, color1)
      .addVertex(x2, y1, 0, color1);

    context.flatBatch
      .addIndices(0, 1, 2, 2, 3, 0)
      .incrementIndices(4);

    //

    context.flatBatch
      .addVertex(x1, y1, elevation, color2)
      .addVertex(x1, y2, elevation, color2)
      .addVertex(x1, y2, 0, color2)
      .addVertex(x1, y1, 0, color2);

    context.flatBatch
      .addIndices(0, 1, 2, 2, 3, 0)
      .incrementIndices(4);

    context.flatBatch
      .addVertex(x2, y2, elevation, color2)
      .addVertex(x2, y1, elevation, color2)
      .addVertex(x2, y1, 0, color2)
      .addVertex(x2, y2, 0, color2);

    context.flatBatch
      .addIndices(0, 1, 2, 2, 3, 0)
      .incrementIndices(4);
  }
}

/*
 * ELEVATION (CAP)
 */

void TextSpan::drawPass1(DrawingContext &context)
{
  if (elevation >= 0)
  {
    glm::vec4 color(PALETTE[id], 1);

    if (!opened)
    {
      context.flatBatch
        .addVertex(x1, y1, elevation, color)
        .addVertex(x2, y1, elevation, color)
        .addVertex(x2, y2, elevation, color)
        .addVertex(x1, y2, elevation, color);

      context.flatBatch
        .addIndices(0, 1, 2, 2, 3, 0)
        .incrementIndices(4);
    }
    else
    {
      float w = x2 - x1;
      float h = (y2 - y1) * 0.5f;

      Matrix matrix;

      matrix
        .translate(x1, y2, elevation)
        .rotateX(angleBottom);

      matrix.addTransformedQuad(Quad<XYZ.RGBA>(0, 0, w, -h, color), context.flatBatch);

      matrix
        .setIdentity()
        .translate(x1, y1, elevation)
        .rotateX(angleTop);

      matrix.addTransformedQuad(Quad<XYZ.RGBA>(0, 0, w, h, color), context.flatBatch);
    }
  }
}

/*
 * REGULAR-CAP TEXT
 */

void TextSpan::drawPass2(DrawingContext &context)
{
  if (!opened)
  {
    if (elevation >= 0)
    {
      float x = x1 - OX;
      float y = y1 + font->getAscent() - OY;

      for (auto c : text)
      {
        int glyphIndex = font->getGlyphIndex(c);
        font->addGlyph(glyphIndex, x, y, elevation);
        x += font->getGlyphAdvance(glyphIndex);
      }
    }
  }
}

/*
 * OPENNING-CAP TEXT
 */

void TextSpan::drawPass3(DrawingContext &context)
{
  if (opened)
  {
    float w = x2 - x1;
    float h = y2 - y1;

    Matrix matrix;

    matrix
      .translate(x1, y2, elevation)
      .rotateX(angleBottom);

    font->setClip(0, -h * 0.5f, w, 0);
    drawText(matrix, -OX, -font->getDescent() + OY);

    matrix
      .setIdentity()
      .translate(x1, y1, elevation)
      .rotateX(angleTop);

    font->setClip(0, 0, w, h * 0.5f);
    drawText(matrix, -OX, +font->getAscent() - OY);

    font->clearClip();
  }
}

/*
 * CAMOUFLAGE
 */

void TextSpan::drawPass4(DrawingContext &context)
{
  float f = 1 / (context.texture.width * textureZoom);
  float tx3 = textureOffsetX;
  float tx1 = tx3 + f * (y2 - y1);
  float tx2 = tx1 + f * (x2 - x1);
  float tx4 = tx2 + f * (y2 - y1);
  float ty1 = textureOffsetY;
  float ty2 = ty1 + elevation / (context.texture.width * textureZoom);

  context.textureBatch
    .addVertex(x1, y2, elevation, glm::vec2(tx1, ty1))
    .addVertex(x2, y2, elevation, glm::vec2(tx2, ty1))
    .addVertex(x2, y2, 0, glm::vec2(tx2, ty2))
    .addVertex(x1, y2, 0, glm::vec2(tx1, ty2));

  context.textureBatch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);

  context.textureBatch
    .addVertex(x2, y1, elevation, glm::vec2(tx1, ty1))
    .addVertex(x1, y1, elevation, glm::vec2(tx2, ty1))
    .addVertex(x1, y1, 0, glm::vec2(tx2, ty2))
    .addVertex(x2, y1, 0, glm::vec2(tx1, ty2));

  context.textureBatch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);

  context.textureBatch
    .addVertex(x1, y1, elevation, glm::vec2(tx3, ty1))
    .addVertex(x1, y2, elevation, glm::vec2(tx1, ty1))
    .addVertex(x1, y2, 0, glm::vec2(tx1, ty2))
    .addVertex(x1, y1, 0, glm::vec2(tx3, ty2));

  context.textureBatch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);

  context.textureBatch
    .addVertex(x2, y2, elevation, glm::vec2(tx2, ty1))
    .addVertex(x2, y1, elevation, glm::vec2(tx4, ty1))
    .addVertex(x2, y1, 0, glm::vec2(tx4, ty2))
    .addVertex(x2, y2, 0, glm::vec2(tx2, ty2));

  context.textureBatch
    .addIndices(0, 1, 2, 2, 3, 0)
    .incrementIndices(4);
}

/*
 * OUTLINES
 */

void TextSpan::drawPass5(DrawingContext &context)
{
  if (elevation > 0)
  {
    context.lineBatch
      .addVertex(x1, y1, 0).addVertex(x1, y1, elevation)
      .addVertex(x2, y1, 0).addVertex(x2, y1, elevation)
      .addVertex(x2, y2, 0).addVertex(x2, y2, elevation)
      .addVertex(x1, y2, 0).addVertex(x1, y2, elevation);

    context.lineBatch
      .addVertex(x1, y1, elevation).addVertex(x2, y1, elevation)
      .addVertex(x2, y1, elevation).addVertex(x2, y2, elevation)
      .addVertex(x2, y2, elevation).addVertex(x1, y2, elevation)
      .addVertex(x1, y2, elevation).addVertex(x1, y1, elevation);
  }
}

// --- ACTIONS ---

void TextSpan::elevate(float elapsed)
{
  t0 = elapsed;
  elevating = true;
}

void TextSpan::collapse(float elapsed)
{
  collapseElevationStart = elevation;
  collapseElevationEnd = elevation - collapseAmount;
  collapseT0 = elapsed;
  collapsing = true;
}

void TextSpan::open(float elapsed)
{
  t0 = elapsed;
  opened = true;
  rotating = true;
  rotationDirection = +1;
}

void close(float elapsed)
{}

void TextSpan::launch(float elapsed, shared_ptr<Missile> missile)
{
  if (!launching)
  {
    this->missile = missile;
    launching = true;
    open(elapsed);
  }
}

void TextSpan::drawText(const chr::gl::Matrix &matrix, float x, float y)
{
  for (auto c : text)
  {
    int glyphIndex = font->getGlyphIndex(c);
    font->addGlyph(matrix, glyphIndex, x, y);
    x += font->getGlyphAdvance(glyphIndex);
  }
}
