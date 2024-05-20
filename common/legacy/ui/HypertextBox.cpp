#include "HypertextBox.h"
#include "TouchableLink.h"

namespace chr
{
    using namespace std;
    using namespace gl;

    void HypertextBox::setLocation(float x, float y)
    {
        if ((x != this->x) || (y != this->y))
        {
            updateLocationRequest = true;
            Shape::setLocation(x, y);
        }
    }

    void HypertextBox::setText(const u16string &text)
    {
        auto extracted = LinkExtractor::extract(text);
        links = extracted.first;
        TextBox::setText(extracted.second);
    }

    bool HypertextBox::layout()
    {
        bool result = TextBox::layout();

        if (result || updateLocationRequest)
        {
            linkSpans.clear();
            for (const auto &link : links)
            {
                linkSpans.push_back(getLinkSpan(link));
            }

            // ---

            touchableLinks.clear();

            float offsetTop = font->getSize() * linkPaddingFactor;
            float offsetBottom = font->getHeight() + offsetTop;

            int index = 0;
            for (const auto &linkSpan : linkSpans)
            {
                for (const auto &position : linkSpan.positions)
                {
                    glm::vec2 p1 = position.first;
                    glm::vec2 p2 = position.second;
                    math::Rectf bounds(glm::vec2(p1.x, p1.y - offsetTop), glm::vec2(p2.x, p2.y + offsetBottom));

                    touchableLinks.push_back(new TouchableLink(this, index++, bounds, Touchable::TYPE_PRESSABLE, linkHitExtra));
                }
            }

            updateLocationRequest = false;
            return true;
        }

        return false;
    }

    void HypertextBox::draw()
    {
        layout();
        TextBox::draw();
        drawLinkUnderlines();
    }

    void HypertextBox::setColorShader(const shaders::ColorShader &shader)
    {
        lineBatch
            .setPrimitive(GL_LINES)
            .setShader(shader);
    }

    vector<Touchable*> HypertextBox::getTouchables()
    {
        vector<Touchable*> touchables;

        for (auto &touchableLink : touchableLinks)
        {
            touchables.push_back(&touchableLink);
        }

        return touchables;
    }

    void HypertextBox::touchActionPerformed(Touchable *touchable, Touchable::Action action)
    {
        if (handler)
        {
            TouchableLink *touchableLink = static_cast<TouchableLink*>(touchable);
            handler->linkClicked(links[touchableLink->linkIndex].url);
        }
    }

    LinkSpan HypertextBox::getLinkSpan(const ExtractedLink &link)
    {
        int startIndex = link.offset;
        int endIndex = link.offset + link.length;

        int startLine = wrapper.getLine(startIndex);
        int endLine = wrapper.getLine(endIndex);

        int n = endLine - startLine + 1;

        LinkSpan linkSpan(startIndex, endIndex, startLine, endLine);

        for (int i = 0; i < n; i++)
        {
            glm::vec2 p1;
            glm::vec2 p2;

            if (i == 0)
            {
                p1 = getLocationAt(startLine, startIndex);

                if (n == 1)
                {
                    p2 = getLocationAt(endLine, endIndex);
                }
                else
                {
                    p2 = getLocationAt(startLine, numeric_limits<int>::max());
                }
            }
            else if (i == n - 1)
            {
                p1 = getLocationAt(endLine, numeric_limits<int>::min());
                p2 = getLocationAt(endLine, endIndex);
            }
            else
            {
                p1 = getLocationAt(startLine + i, numeric_limits<int>::min());
                p2 = getLocationAt(startLine + i, numeric_limits<int>::max());
            }

            linkSpan.positions.emplace_back(p1, p2);
        }

        return linkSpan;
    }

    void HypertextBox::drawLinkUnderlines()
    {
        lineBatch
            .setShaderColor(textColor)
            .clear();

        float offsetBottom = font->getAscent() + font->getSize() * linkUnderlineFactor;

        for (const auto linkSpan : linkSpans)
        {
            for (const auto position : linkSpan.positions)
            {
                glm::vec2 p1 = position.first;
                glm::vec2 p2 = position.second;

                p1.y = floorf(p1.y + offsetBottom);
                p2.y = floorf(p2.y + offsetBottom);

                lineBatch
                    .addVertex(p1)
                    .addVertex(p2);
            }
        }

        lineBatch.flush();
    }
}
