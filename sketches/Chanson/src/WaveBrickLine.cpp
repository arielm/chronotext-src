#include "WaveBrickLine.h"

#include "common/cinder/Easing.h"

using namespace std;
using namespace chr;
using namespace gl;

WaveBrickLine::WaveBrickLine(float brickHeight, float brickDepth, float brickPadding, float brickMargin)
:
brickHeight(brickHeight),
brickDepth(brickDepth),
brickPadding(brickPadding),
brickMargin(brickMargin)
{}

void WaveBrickLine::updateIndices(vector<GLuint> &capIndices, vector<GLuint> &wallIndices, vector<GLuint> &strokeIndices)
{
  int index = verticesIndex;

  for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible && (it->partialSegmentCount > 0))
    {
      for (int i = 0; i < it->partialSegmentCount; i++)
      {
        int i0 = index + 3;
        int i1 = index + 2;
        int i2 = index + 6;
        int i3 = index + 7;

        capIndices.push_back(i0);
        capIndices.push_back(i1);
        capIndices.push_back(i2);
        capIndices.push_back(i2);
        capIndices.push_back(i3);
        capIndices.push_back(i0);

        index += 4;
      }

      index += 4;
    }
  }

  // ---

  index = verticesIndex;

  for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible)
    {
      if (it->partialSegmentCount > 0)
      {
        int i0 = index + 3;
        int i1 = index + 1;
        int i2 = index + 0;
        int i3 = index + 2;

        wallIndices.push_back(i0);
        wallIndices.push_back(i1);
        wallIndices.push_back(i2);
        wallIndices.push_back(i2);
        wallIndices.push_back(i3);
        wallIndices.push_back(i0);
      }

      for (int i = 0; i < it->partialSegmentCount; i++)
      {
        int k0 = index + 2;
        int k1 = index + 0;
        int k2 = index + 4;
        int k3 = index + 6;

        wallIndices.push_back(k0);
        wallIndices.push_back(k1);
        wallIndices.push_back(k2);
        wallIndices.push_back(k2);
        wallIndices.push_back(k3);
        wallIndices.push_back(k0);

        // ---

        int l0 = index + 3;
        int l1 = index + 7;
        int l2 = index + 5;
        int l3 = index + 1;

        wallIndices.push_back(l0);
        wallIndices.push_back(l1);
        wallIndices.push_back(l2);
        wallIndices.push_back(l2);
        wallIndices.push_back(l3);
        wallIndices.push_back(l0);

        index += 4;
      }

      if (it->partialSegmentCount > 0)
      {
        int j0 = index + 3;
        int j1 = index + 2;
        int j2 = index + 0;
        int j3 = index + 1;

        wallIndices.push_back(j0);
        wallIndices.push_back(j1);
        wallIndices.push_back(j2);
        wallIndices.push_back(j2);
        wallIndices.push_back(j3);
        wallIndices.push_back(j0);

        index += 4;
      }
    }
  }

  // ---

  index = verticesIndex;

  for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible)
    {
      if (it->partialSegmentCount > 0)
      {
        int i0 = index + 3;
        int i1 = index + 1;
        int i2 = index + 0;
        int i3 = index + 2;

        strokeIndices.push_back(i0);
        strokeIndices.push_back(i1);

        strokeIndices.push_back(i1);
        strokeIndices.push_back(i2);

        strokeIndices.push_back(i2);
        strokeIndices.push_back(i3);

        strokeIndices.push_back(i3);
        strokeIndices.push_back(i0);

        index += it->partialSegmentCount * 4;

        int j0 = index + 3;
        int j1 = index + 1;
        int j2 = index + 0;
        int j3 = index + 2;

        strokeIndices.push_back(j0);
        strokeIndices.push_back(j1);

        strokeIndices.push_back(j1);
        strokeIndices.push_back(j2);

        strokeIndices.push_back(j2);
        strokeIndices.push_back(j3);

        strokeIndices.push_back(j3);
        strokeIndices.push_back(j0);

        index += 4;
      }
    }
  }

  const int indices[] = {0, 2, 1, 3};

  for (int i = 0; i < 4; i++)
  {
    index = verticesIndex + indices[i];

    for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
    {
      if (it->visible && (it->partialSegmentCount > 0))
      {
        for (int i = 0; i < it->partialSegmentCount; i++)
        {
          strokeIndices.push_back(index);
          strokeIndices.push_back(index + 4);

          index += 4;
        }

        index += 4;
      }
    }
  }
}

void WaveBrickLine::updateVertices(vector<Vertex<>> &vertices, float scrollingOffset)
{
  verticesIndex = vertices.size();

  Matrix matrix;
  float remainder;

  /*
   * WE MUST ADD SOME EXTRA, TO AVOID VISUAL ARTIFACTS TOWARD THE END OF THE LINE
   * SIDE-EFFECT: SLIGHT REDUCTION OF THE "MAXIMUM BRICK DEPTH" (I.E. TOWARD THE END)
   */
  float endPosition = lineLength * 1.1f;

  for (vector<Brick3>::iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->end > it->start)
    {
      remainder = endPosition - (it->position + it->start * (it->width - brickMargin));
      break;
    }
  }

  for (vector<Brick3>::iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible)
    {
      it->path.begin();

      float halfHeight = brickHeight * 0.5f;
      float bottom = 0;

      if ((it->start == 0) && (it->end == 1))
      {
        for (int i = 0; i <= it->fullSegmentCount; i++)
        {
          float position = it->position + i * (it->width - brickMargin) / it->fullSegmentCount;
          float u = 1 - (endPosition - position) / remainder;
          float top = bottom - brickDepth * cinder::easeOutAtan(u);

          auto value = waveLine->path.offsetToValue(scrollingOffset + shift + position + brickMargin * 0.5f);
          value.applyToMatrix(matrix);

          vertices.emplace_back(matrix.transformPoint(0, +halfHeight, bottom));
          vertices.emplace_back(matrix.transformPoint(0, -halfHeight, bottom));

          vertices.emplace_back(matrix.transformPoint(0, +halfHeight, top));
          vertices.emplace_back(matrix.transformPoint(0, -halfHeight, top));

          auto transformed = matrix.transformPoint(0, 0, top);
          it->path.add(transformed, value.left);
        }

        it->partialSegmentCount = it->fullSegmentCount;
      }
      else if (it->end > it->start)
      {
        /*
         * EVEN IF WE DRAW A PARTIAL BRICK, WE NEED A Path3D
         * INCLUDING THE WHOLE BRICK FOR PROPER TEXT RENDERING
         */
        for (int i = 0; i <= it->fullSegmentCount; i++)
        {
          float position = it->position + i * (it->width - brickMargin) / it->fullSegmentCount;
          float u = 1 - (endPosition - position) / remainder;
          float top = bottom - brickDepth * cinder::easeOutAtan(u);

          auto value = waveLine->path.offsetToValue(scrollingOffset + shift + position + brickMargin * 0.5f);
          value.applyToMatrix(matrix);

          auto transformed = matrix.transformPoint(0, 0, top);
          it->path.add(transformed, value.left);
        }

        float start = it->start * (it->width - brickMargin);
        float width = (it->width - brickMargin) * (it->end - it->start);
        int segmentCount = max<int>(1, width / segmentLength);

        for (int i = 0; i <= segmentCount; i++)
        {
          float position = it->position + start + i * width / segmentCount;
          float u = 1 - (endPosition - position) / remainder;
          float top = bottom - brickDepth * cinder::easeOutAtan(u);

          auto value = waveLine->path.offsetToValue(scrollingOffset + shift + position + brickMargin * 0.5f);
          value.applyToMatrix(matrix);

          vertices.emplace_back(matrix.transformPoint(0, +halfHeight, bottom));
          vertices.emplace_back(matrix.transformPoint(0, -halfHeight, bottom));

          vertices.emplace_back(matrix.transformPoint(0, +halfHeight, top));
          vertices.emplace_back(matrix.transformPoint(0, -halfHeight, top));
        }

        it->partialSegmentCount = segmentCount;
      }
      else
      {
        it->partialSegmentCount = 0;
      }

      it->path.end();
    }
  }
}

/*
 * SIMILAR TO ThinWaveBrickLine::drawText()
 */
void WaveBrickLine::drawText(XFont &font, float fontSize, float polygonOffset)
{
  Matrix matrix;

  for (vector<Brick3>::iterator it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->visible && it->hasText)
    {
      float ratio =  it->path.getLength() / (it->width - brickMargin);
      font.setSize(fontSize * ratio);

      float baselineOffset = font.getOffsetY(XFont::ALIGN_MIDDLE);
      float position = brickPadding / ratio;

      for (auto c : it->text)
      {
        int glyphIndex = font.getGlyphIndex(c);

        float half = font.getGlyphAdvance(glyphIndex) * 0.5f;
        position += half;

        if (glyphIndex >= 0)
        {
          auto value = it->path.offsetToValue(position);
          value.applyToMatrix(matrix);

          if (polygonOffset != 0)
          {
            matrix.translate(0, 0, -polygonOffset);
          }

          if ((it->start == 0) && (it->end == 1))
          {
            font.addGlyph(matrix, glyphIndex, -half, baselineOffset);
          }
          else if (it->end > it->start)
          {
            float x1 = it->start * (it->width - brickMargin) - position / ratio;
            float x2 = it->end * (it->width - brickMargin) - position / ratio;

            font.setClip(x1, -9999, x2, +9999);
            font.addGlyph(matrix, glyphIndex, -half, baselineOffset);
            font.clearClip();
          }
        }

        position += half;
      }
    }
  }
}
