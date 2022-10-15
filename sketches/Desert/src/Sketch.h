#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/ShaderProgram.h"

#include "common/legacy/NoisePatch.h"

class Sketch : public chr::CrossSketch
{
public:
    Sketch();
    virtual ~Sketch() {}

    void setup() final;
    void update() final;
    void draw() final;

protected:
    chr::gl::ShaderProgram fogColorShader;

    NoiseSurface surface;
    NoisePatch patch;

    float viewOX = 0;
    float viewOY = 0;
};
