#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/Random.h"

#include "common/xf/FontManager.h"

class Sketch : public chr::CrossSketch
{
public:
    void setup() final;
    void resize() final;
    void draw() final;

protected:
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;
    chr::gl::shaders::ColorShader colorShader;

    chr::gl::VertexBatch<chr::gl::XYZ> strokeBatch;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ> fillBatch;

    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence;

    float scale;

    chr::Random rnd1, rnd2;
    std::vector<std::u16string> words;
    int wordCount;

    void generate();
    std::u16string generateLine(int numWords);

    static void drawCircularText(chr::XFont &font, const std::u16string &text, const glm::vec2 &position, float radius, float offset, chr::XFont::Alignment alignY);

    void drawDot();
    void drawCircles();
    void drawCircle(float x, float y, float r, float segmentLength, float a1 = 0, float a2 = chr::TWO_PI);
};
