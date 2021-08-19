//
//  ButtonBar.cpp
//  TextNoise
//
//  Created by Ariel Malka on 4/4/20.
//  Copyright © 2020 chronotext.com. All rights reserved.
//

#include "ButtonBar.h"

#include "chr/gl/Matrix.h"
#include "chr/gl/draw/Sprite.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

static constexpr float BUTTON_SPACER_W = 24;
static constexpr float BUTTON_EXTRA = 8;

void ButtonBar::init(const Properties &properties, const vector<string> &names)
{
    this->properties = properties;
    
    for (auto name : names)
    {
        textures.push_back(
            Texture::ImageRequest(name)
                .setFlags(image::FLAGS_TRANSLUCENT)
                .setMipmap(true));
    }
    
    buttonCount = textures.size();
    buttonWidth = textures[0].width * 0.5f * properties.contentScale;
    buttonHeight = textures[0].height * 0.5f * properties.contentScale;
    
    float buttonsWidth = buttonWidth * buttonCount + BUTTON_SPACER_W * properties.contentScale * (buttonCount - 1);
    x = (properties.windowWidth - buttonsWidth) * 0.5f;
    y = properties.toolBarTop + (properties.height * properties.contentScale - buttonHeight) * 0.5f;
    
    float xx = x;
    for (auto i = 0; i < buttonCount; i++)
    {
        rectangles.emplace_back(xx, y, buttonWidth, buttonHeight);
        xx += buttonWidth + BUTTON_SPACER_W * properties.contentScale;
    }
}

void ButtonBar::draw(IndexedVertexBatch<XYZ.UV> &batch)
{
    Matrix matrix;
    
    for (auto i = 0; i < buttonCount; i++)
    {
        batch.clear();
        
        matrix
          .setTranslate(rectangles[i].x1y1())
          .scale(0.5f * properties.contentScale);
        
        draw::Sprite().append(batch, matrix);
        
        batch
          .setTexture(textures[i])
          .flush();
    }
}

int ButtonBar::checkTouch(float x, float y)
{
    int i = 0;
    for (auto &rectangle : rectangles)
    {
        float extra = BUTTON_EXTRA * properties.contentScale;
        
        if (rectangle.inflated(glm::vec2(extra, extra)).contains(x, y))
        {
            return i;
        }
        
        i++;
    }
    
    return -1;
}
