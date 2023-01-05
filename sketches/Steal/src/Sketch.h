#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/ShaderProgram.h"

#include "common/xf/FontManager.h"
#include "common/Noise.h"

class Sketch : public chr::CrossSketch
{
public:
    Sketch();

    void setup() final;
    void draw() final;

protected:
    chr::gl::ShaderProgram fogColorShader;
    chr::gl::ShaderProgram fogTextureAlphaShader;

    chr::gl::IndexedVertexBatch<chr::gl::XYZ> fillBatch;

    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::XFont> font;
    chr::xf::FontSequence sequence;

    Noise noise;

    void generate(const chr::gl::Matrix &matrix, float ox, float oy);
};
