#include "ThinFlatBrickLine.h"

using namespace std;
using namespace chr;

ThinFlatBrickLine::ThinFlatBrickLine(float brickHeight, float brickPadding, float brickMargin)
:
brickHeight(brickHeight),
brickPadding(brickPadding),
brickMargin(brickMargin)
{}

void ThinFlatBrickLine::updateIndices(vector<GLuint> &strokeIndices)
{
    int index = verticesIndex;

    for (vector<Brick3>::const_iterator it = bricks.begin(); it != bricks.end(); ++it)
    {
        if (it->visible)
        {
            int i0 = index + 0;
            int i1 = index + 1;
            int i2 = index + 2;
            int i3 = index + 3;

            // ---

            strokeIndices.push_back(i0);
            strokeIndices.push_back(i1);

            strokeIndices.push_back(i2);
            strokeIndices.push_back(i3);

            // ---

            strokeIndices.push_back(i0);
            strokeIndices.push_back(i2);

            strokeIndices.push_back(i1);
            strokeIndices.push_back(i3);

            // ---

            index += 4;
        }
    }
}

void ThinFlatBrickLine::updateVertices(vector<Vertex<>> &vertices, float scrollingOffset)
{
    verticesIndex = vertices.size();

    for (vector<Brick3>::iterator it = bricks.begin(); it != bricks.end(); ++it)
    {
        if (it->visible)
        {
            float position = scrollingOffset + shift + it->position + brickMargin * 0.5f;

            float positionStart = position + it->start * (it->width - brickMargin);
            float positionEnd = positionStart + (it->width - brickMargin) * (it->end - it->start);

            float halfHeight = brickHeight * 0.5f;

            vertices.emplace_back(positionStart, y + halfHeight, 0);
            vertices.emplace_back(positionStart, y - halfHeight, 0);

            vertices.emplace_back(positionEnd, y + halfHeight, 0);
            vertices.emplace_back(positionEnd, y - halfHeight, 0);

            it->anchor = glm::vec3(position, y, 0);
        }
    }
}

void ThinFlatBrickLine::drawText(XFont &font, float fontSize)
{
    Matrix matrix;

    font.setSize(fontSize);
    float baselineOffset = font.getOffsetY(XFont::ALIGN_MIDDLE);

    for (vector<Brick3>::iterator it = bricks.begin(); it != bricks.end(); ++it)
    {
        if (it->visible && it->hasText)
        {
            matrix.setTranslate(it->anchor);

            float position = brickPadding;

            if ((it->start == 0) && (it->end == 1))
            {
                for (auto c : it->text)
                {
                    int glyphIndex = font.getGlyphIndex(c);
                    font.addGlyph(matrix, glyphIndex, position, baselineOffset);
                    position += font.getGlyphAdvance(glyphIndex);
                }
            }
            else
            {
                float x1 = it->start * (it->width - brickMargin);
                float x2 = it->end * (it->width - brickMargin);
                font.setClip(x1, -9999, x2, +9999);

                for (auto c : it->text)
                {
                    int glyphIndex = font.getGlyphIndex(c);
                    font.addGlyph(matrix, glyphIndex, position, baselineOffset);
                    position += font.getGlyphAdvance(glyphIndex);
                }

                font.clearClip();
            }
        }
    }
}
