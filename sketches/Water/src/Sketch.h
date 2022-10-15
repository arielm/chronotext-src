#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/ShaderProgram.h"

#include "common/legacy/SeaPatch.h"

class Sketch : public chr::CrossSketch
{
public:
    Sketch();

    void setup() final;
    void update() final;
    void draw() final;

protected:
    chr::gl::ShaderProgram fogColorShader;

    SeaSurface surface;
    SeaPatch patch;
};
