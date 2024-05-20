#pragma once

#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"

#include "TextBox.h"
#include "LinkExtractor.h"

#include <boost/ptr_container/ptr_vector.hpp>

namespace chr
{
    struct LinkSpan
    {
        int startIndex;
        int endIndex;

        int startLine;
        int endLine;

        std::vector< std::pair<glm::vec2, glm::vec2> > positions;

        LinkSpan(int startIndex, int endIndex, int startLine, int endLine)
        :
        startIndex(startIndex),
        endIndex(endIndex),
        startLine(startLine),
        endLine(endLine)
        {}
    };

    class HypertextBox : public TextBox
    {
    public:
        class Handler
        {
        public:
            ~Handler() {}
            virtual void linkClicked(const std::string &url) = 0;
        };

        Handler *handler = nullptr;

        HypertextBox() = default;

        void setLocation(float x, float y) override;
        void setText(const std::u16string &text) override;
        void setColorShader(const gl::shaders::ColorShader &shader);

        bool layout() override;
        void draw() override;

        std::vector<Touchable*> getTouchables() override;
        void touchActionPerformed(Touchable *touchable, Touchable::Action action) override;

    protected:
        gl::VertexBatch<gl::XYZ> lineBatch;

        std::vector<ExtractedLink> links;
        std::vector<LinkSpan> linkSpans;
        boost::ptr_vector<Touchable> touchableLinks;

        bool updateLocationRequest;

        float linkUnderlineFactor = 0.2f;
        float linkPaddingFactor = 0.1f;
        float linkHitExtra = 8;

        LinkSpan getLinkSpan(const ExtractedLink &link);
        void drawLinkUnderlines();
    };
}
