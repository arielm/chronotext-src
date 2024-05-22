#include "ThinWaveBrickLine.h"

using namespace std;
using namespace chr;

ThinWaveBrickLine::ThinWaveBrickLine(float brickHeight, float brickPadding, float brickMargin)
:
brickHeight(brickHeight),
brickPadding(brickPadding),
brickMargin(brickMargin)
{}

void ThinWaveBrickLine::updateIndices(vector<GLuint> &strokeIndices)
{
    int index = verticesIndex;

    for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
    {
        if (it->visible)
        {
            if (it->partialSegmentCount > 0)
            {
                int i0 = index + 0;
                int i1 = index + 1;

                strokeIndices.push_back(i0);
                strokeIndices.push_back(i1);

                index += it->partialSegmentCount * 2;

                int j0 = index + 0;
                int j1 = index + 1;

                strokeIndices.push_back(j0);
                strokeIndices.push_back(j1);

                index += 2;
            }
        }
    }

    const int indices[] = {0, 1};

    for (int i = 0; i < 2; i++)
    {
        index = verticesIndex + indices[i];

        for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
        {
            if (it->visible && (it->partialSegmentCount > 0))
            {
                for (int i = 0; i < it->partialSegmentCount; i++)
                {
                    strokeIndices.push_back(index);
                    strokeIndices.push_back(index + 2);

                    index += 2;
                }

                index += 2;
            }
        }
    }
}

void ThinWaveBrickLine::updateVertices(vector<Vertex<>> &vertices, float scrollingOffset)
{
    verticesIndex = vertices.size();

    Matrix matrix;

    for (vector<Brick3>::iterator it = bricks.begin(); it != bricks.end(); ++it)
    {
        if (it->visible)
        {
            it->path.begin();

            float halfHeight = brickHeight * 0.5f;
            float top = 0;

            if ((it->start == 0) && (it->end == 1))
            {
                for (int i = 0; i <= it->fullSegmentCount; i++)
                {
                    float position = it->position + i * (it->width - brickMargin) / it->fullSegmentCount;
                    auto value = waveLine->path.offsetToValue(scrollingOffset + shift + position + brickMargin * 0.5f);
                    value.applyToMatrix(matrix);

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
                    auto value = waveLine->path.offsetToValue(scrollingOffset + shift + position + brickMargin * 0.5f);
                    value.applyToMatrix(matrix);

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
 * SIMILAR TO WaveBrickLine::drawText()
 */
void ThinWaveBrickLine::drawText(XFont &font, float fontSize, float polygonOffset)
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
