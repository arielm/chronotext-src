#include "SeaBrickSpiral.h"

using namespace std;
using namespace chr;
using namespace gl;

SeaBrickSpiral::SeaBrickSpiral(float brickHeight, float brickDepth, float brickPadding, float brickMargin)
:
brickHeight(brickHeight),
brickDepth(brickDepth),
brickPadding(brickPadding),
brickMargin(brickMargin)
{}

void SeaBrickSpiral::update(SeaSurfaceSpiral2 &spiral)
{
  updateVertices(spiral);
  updateCapIndices();
  updateWallIndices();
  updateStrokeIndices();
}

void SeaBrickSpiral::updateCapIndices()
{
  auto &capIndices = capIndexBuffer->storage;
  capIndices.clear();
  capIndexBuffer.requestUpload();

  int index = 0;

  for (auto &brick : bricks)
  {
    if (brick.partialSegmentCount > 0)
    {
      for (int i = 0; i < brick.partialSegmentCount; i++)
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
}

void SeaBrickSpiral::updateWallIndices()
{
  auto &wallIndices = wallIndexBuffer->storage;
  wallIndices.clear();
  wallIndexBuffer.requestUpload();

  int index = 0;
  
  for (auto &brick : bricks)
  {
    if (brick.partialSegmentCount > 0)
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
    
    for (int i = 0; i < brick.partialSegmentCount; i++)
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
    
    if (brick.partialSegmentCount > 0)
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

void SeaBrickSpiral::updateStrokeIndices()
{
  auto &strokeIndices = strokeIndexBuffer->storage;
  strokeIndices.clear();
  strokeIndexBuffer.requestUpload();

  int index = 0;

  for (auto &brick : bricks)
  {
    if (brick.partialSegmentCount > 0)
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

      index += brick.partialSegmentCount * 4;

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

  const int indices[] = {0, 2, 1, 3};

  for (int i = 0; i < 4; i++)
  {
    index = indices[i];

    for (auto &brick : bricks)
    {
      if (brick.partialSegmentCount > 0)
      {
        for (int i = 0; i < brick.partialSegmentCount; i++)
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

void SeaBrickSpiral::updateVertices(SeaSurfaceSpiral2 &spiral)
{
  auto &vertices = vertexBuffer->storage;
  vertices.clear();
  vertexBuffer.requestUpload();

  Matrix matrix;

  float l = TWO_PI * spiral.turns;
  float dr = (spiral.r2 - spiral.r1) / l;
  float DD = (spiral.DD2 - spiral.DD1) / l;

  for (auto &brick : bricks)
  {
    brick.path.begin();

    float halfHeight = brickHeight * 0.5f;
    float top = brickDepth * brick.factor;

    if ((brick.start == 0) && (brick.end == 1))
    {
      for (int i = 0; i <= brick.fullSegmentCount; i++)
      {
        float position = brick.position + i * (brick.width - brickMargin) / brick.fullSegmentCount;
        auto value = spiral.path.offsetToValue(position + brickMargin * 0.5f);
        value.applyToMatrix(matrix);

        // ---

        vertices.emplace_back(matrix.transformPoint(0, +halfHeight, 0));
        vertices.emplace_back(matrix.transformPoint(0, -halfHeight, 0));

        vertices.emplace_back(matrix.transformPoint(0, +halfHeight, -top));
        vertices.emplace_back(matrix.transformPoint(0, -halfHeight, -top));

        auto transformed = matrix.transformPoint(0, 0, -top);
        brick.path.add(transformed, value.left);
      }

      brick.partialSegmentCount = brick.fullSegmentCount;
    }
    else if (brick.end > brick.start)
    {
      /*
       * EVEN IF WE DRAW A PARTIAL BRICK, WE NEED A Path3D
       * INCLUDING THE WHOLE BRICK FOR PROPER TEXT RENDERING
       */
      for (int i = 0; i <= brick.fullSegmentCount; i++)
      {
        float position = brick.position + i * (brick.width - brickMargin) / brick.fullSegmentCount;
        auto value = spiral.path.offsetToValue(position + brickMargin * 0.5f);
        value.applyToMatrix(matrix);

        auto transformed = matrix.transformPoint(0, 0, -top);
        brick.path.add(transformed, value.left);
      }

      float start = brick.start * (brick.width - brickMargin);
      float width = (brick.width - brickMargin) * (brick.end - brick.start);

      float r = sqrtf(spiral.r1 * spiral.r1 + 2 * dr * (brick.position + brickMargin * 0.5f + start));
      float d = (r - spiral.r1) / dr;

      float segmentLength = spiral.DD1 + d * DD;
      int segmentCount = max<int>(1, width / segmentLength);

      for (int i = 0; i <= segmentCount; i++)
      {
        float position = brick.position + start + i * width / segmentCount;
        spiral.path
          .offsetToValue(position + brickMargin * 0.5f)
          .applyToMatrix(matrix);

        // ---

        vertices.emplace_back(matrix.transformPoint(0, +halfHeight, 0));
        vertices.emplace_back(matrix.transformPoint(0, -halfHeight, 0));

        vertices.emplace_back(matrix.transformPoint(0, +halfHeight, -top));
        vertices.emplace_back(matrix.transformPoint(0, -halfHeight, -top));
      }

      brick.partialSegmentCount = segmentCount;
    }
    else
    {
      brick.partialSegmentCount = 0;
    }

    brick.path.end();
  }
}

/*
 * SIMILAR TO WaveBrickLine::drawText() AND ThinWaveBrickLine::drawText()
 */
void SeaBrickSpiral::drawText(XFont &font, float fontSize, float polygonOffset)
{
  Matrix matrix;

  for (auto &brick : bricks)
  {
    if (brick.hasText)
    {
      float ratio =  brick.path.getLength() / (brick.width - brickMargin);
      font.setSize(fontSize * ratio);

      float baselineOffset = font.getOffsetY(XFont::ALIGN_MIDDLE);
      float position = brickPadding / ratio;

      for (auto c : brick.text)
      {
        int glyphIndex = font.getGlyphIndex(c);

        float half = font.getGlyphAdvance(glyphIndex) * 0.5f;
        position += half;

        if (glyphIndex >= 0)
        {
          brick.path
            .offsetToValue(position)
            .applyToMatrix(matrix);

          if (polygonOffset != 0)
          {
            matrix.translate(0, 0, -polygonOffset);
          }

          if ((brick.start == 0) && (brick.end == 1))
          {
            font.addGlyph(matrix, glyphIndex, -half, baselineOffset);
          }
          else if (brick.end > brick.start)
          {
            float x1 = brick.start * (brick.width - brickMargin) - position / ratio;
            float x2 = brick.end * (brick.width - brickMargin) - position / ratio;

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