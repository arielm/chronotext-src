#pragma once

#include "chr/gl/Batch.h"

struct DrawingContext
{
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> flatBatch;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> textureBatch;
  chr::gl::VertexBatch<chr::gl::XYZ> lineBatch;
  chr::gl::VertexBatch<chr::gl::XYZ.RGBA> missileBatch;
  chr::gl::IndexedVertexBatch<chr::gl::XYZ> explosionBatch;

  chr::gl::Texture texture;
};
