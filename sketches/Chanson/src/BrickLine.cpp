#include "BrickLine.h"

using namespace std;
using namespace chr;
using namespace gl;

BrickLine::BrickLine(float brickHeight, float brickDepth, float brickPadding)
:
brickHeight(brickHeight),
brickDepth(brickDepth),
brickPadding(brickPadding),
shift(0)
{}

void BrickLine::updateIndices(vector<GLuint> &capIndices, vector<GLuint> &wallIndices, vector<GLuint> &strokeIndices)
{
  int index = verticesIndex;

  for (vector<Brick2>::iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible)
    {
      int i0 = index + 0;
      int i1 = index + 1;
      int i2 = index + 2;
      int i3 = index + 3;
      int i4 = index + 4;
      int i5 = index + 5;
      int i6 = index + 6;
      int i7 = index + 7;

      // ---

      capIndices.push_back(i1);
      capIndices.push_back(i0);
      capIndices.push_back(i2);
      capIndices.push_back(i2);
      capIndices.push_back(i3);
      capIndices.push_back(i1);

      capIndices.push_back(i7);
      capIndices.push_back(i5);
      capIndices.push_back(i4);
      capIndices.push_back(i4);
      capIndices.push_back(i6);
      capIndices.push_back(i7);

      // ---

      wallIndices.push_back(i1);
      wallIndices.push_back(i7);
      wallIndices.push_back(i6);
      wallIndices.push_back(i6);
      wallIndices.push_back(i0);
      wallIndices.push_back(i1);

      wallIndices.push_back(i5);
      wallIndices.push_back(i3);
      wallIndices.push_back(i2);
      wallIndices.push_back(i2);
      wallIndices.push_back(i4);
      wallIndices.push_back(i5);

      wallIndices.push_back(i1);
      wallIndices.push_back(i3);
      wallIndices.push_back(i5);
      wallIndices.push_back(i5);
      wallIndices.push_back(i7);
      wallIndices.push_back(i1);

      wallIndices.push_back(i0);
      wallIndices.push_back(i6);
      wallIndices.push_back(i4);
      wallIndices.push_back(i4);
      wallIndices.push_back(i2);
      wallIndices.push_back(i0);

      // ---

      strokeIndices.push_back(i0);
      strokeIndices.push_back(i1);
      strokeIndices.push_back(i2);
      strokeIndices.push_back(i3);
      strokeIndices.push_back(i4);
      strokeIndices.push_back(i5);
      strokeIndices.push_back(i6);
      strokeIndices.push_back(i7);

      strokeIndices.push_back(i0);
      strokeIndices.push_back(i6);
      strokeIndices.push_back(i1);
      strokeIndices.push_back(i7);

      strokeIndices.push_back(i2);
      strokeIndices.push_back(i4);
      strokeIndices.push_back(i3);
      strokeIndices.push_back(i5);

      strokeIndices.push_back(i0);
      strokeIndices.push_back(i2);
      strokeIndices.push_back(i6);
      strokeIndices.push_back(i4);

      strokeIndices.push_back(i1);
      strokeIndices.push_back(i3);
      strokeIndices.push_back(i7);
      strokeIndices.push_back(i5);

      // ---

      index += 8;
    }
  }
}

void BrickLine::updateVertices(vector<Vertex<>> &vertices)
{
  verticesIndex = vertices.size();

  Matrix matrix;

  for (vector<Brick2>::iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible)
    {
      float c = it->textWidth + brickPadding * 2;
      float brickLength;

      matrix.setIdentity();

      if (it->hasText)
      {
        float b = (it->v * c + (1 - it->v) * it->slotWidth);
        brickLength = c;

        float ay = acosf(b / c);
        glm::vec3 anchor(it->position + ox + it->marginLeft * it->v, oy, 0);

        matrix.translate(anchor);
        matrix.rotateY(ay);

        it->anchor = anchor;
        it->ay = ay;
      }
      else
      {
        brickLength = it->slotWidth;
        matrix.translate(it->position + ox, oy, 0);
      }

      vertices.emplace_back(matrix.transformPoint(0, 0, 0));
      vertices.emplace_back(matrix.transformPoint(0, -brickHeight, 0));

      vertices.emplace_back(matrix.transformPoint(brickLength, 0, 0));
      vertices.emplace_back(matrix.transformPoint(brickLength, -brickHeight, 0));

      vertices.emplace_back(matrix.transformPoint(brickLength, 0, +brickDepth));
      vertices.emplace_back(matrix.transformPoint(brickLength, -brickHeight, +brickDepth));

      vertices.emplace_back(matrix.transformPoint(0, 0, +brickDepth));
      vertices.emplace_back(matrix.transformPoint(0, -brickHeight, +brickDepth));
    }
  }
}

void BrickLine::drawText(XFont &font, float fontSize)
{
  Matrix matrix;

  font.setSize(fontSize);
  float baselineOffset = font.getOffsetY(XFont::ALIGN_MIDDLE) - brickHeight * 0.5f;

  for (vector<Brick2>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible && it->hasText)
    {
      matrix.setTranslate(it->anchor);
      matrix.rotateY(it->ay);

      float position = brickPadding;

      for (auto c : it->text)
      {
        int glyphIndex = font.getGlyphIndex(c);
        font.addGlyph(matrix, glyphIndex, position, baselineOffset);
        position += font.getGlyphAdvance(glyphIndex);
      }
    }
  }
}
