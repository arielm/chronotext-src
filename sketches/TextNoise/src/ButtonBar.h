//
//  ButtonBar.h
//  TextNoise
//
//  Created by Ariel Malka on 4/4/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#pragma once

#include "chr/gl/Texture.h"
#include "chr/gl/Batch.h"
#include "chr/math/Rect.h"

class ButtonBar
{
public:
    struct Properties
    {
        float windowWidth;
        float height;
        float toolBarTop;
        float contentScale;
    };
    
    void init(const Properties &properties, const std::vector<std::string> &names);
    void draw(chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> &batch);
    int checkTouch(float x, float y);
    
protected:
    std::vector<chr::gl::Texture> textures;
    std::vector<chr::math::Rectf> rectangles;
    
    Properties properties;
    size_t buttonCount;
    float buttonWidth, buttonHeight;
    float x, y;
};
