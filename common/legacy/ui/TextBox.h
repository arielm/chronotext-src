#pragma once

#include "Shape.h"
#include "WordWrapper.h"

namespace chr
{
  class TextBox : public Shape
  {
  public:
    std::shared_ptr<XFont> font;

    float fontSize = 0;
    bool wrap = false;

    float lineHeight = 0;
    float lineHeightFactor = 1;

    XFont::Alignment textAlignX = XFont::ALIGN_LEFT;
    XFont::Alignment textAlignY = XFont::ALIGN_TOP;

    glm::vec4 textColor;

    bool overflowX = false;
    bool overflowY = false;
    float offsetX = 0;
    float offsetY = 0;
    
    std::u16string text;
    WordWrapper wrapper;

    TextBox() = default;

    void setFont(std::shared_ptr<XFont> font);
    void setFontSize(float size);
    void setLineHeightFactor(float factor);
    void setLineHeight(float height);
    void setWidth(float width) override;
    void setHeight(float height) override;
    void setAutoWidth(bool autoWidth) override;
    void setAutoHeight(bool autoHeight) override;
    void setPadding(float left, float top, float right, float bottom) override;
    void setTextAlign(XFont::Alignment x, XFont::Alignment y);
    void setWrap(bool wrap);
    void setOffsetX(float x);
    void setOffsetY(float y);
    void setTextColor(const glm::vec4 &color);
    virtual void setText(const std::u16string &text);
    
    float getWidth() override;
    float getHeight() override;
    float getLineHeight();
    
    bool layout() override;
    void draw() override;
    glm::vec2 getLocationAt(int line, int index);
    
  protected:
    chr::xf::FontSequence sequence;

    bool updateWrapRequest = false;
    bool updateWidthRequest = false;
    bool updateHeightRequest = false;
    bool updateLineHeightRequest = false;
    bool shouldDraw = false;

    float contentWidth = 0;
    float contentHeight = 0;
    
    float getOffsetX(int start, int end);
    float getOffsetY();
    float getLineTop(int index);
    float getLinesHeight(int n);
    
    virtual void updateBatch();
    void drawTextSpan(float xx, float yy, int start, int end, float limitLeft, float limitRight);
  };
}
