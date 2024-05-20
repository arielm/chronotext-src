#pragma once

#include "common/NoiseSurface.h"

#include "chr/gl/Buffer.h"

class NoiseSurfaceGrid
{
public:
    int nx, ny;

    chr::gl::Buffer<chr::gl::Vertex<>> vertexBuffer;
    chr::gl::Buffer<GLuint> indexBuffer;

    void setup(const NoiseSurface &surface, float cx, float cy, float w, float h);
    void update(const NoiseSurface &surface, float height, const glm::vec2 &offset);

    size_t vertexCount();
    size_t indexCount();

protected:
    float ox1, oy1;
    float ox2, oy2;
};
