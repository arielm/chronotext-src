#pragma once

#include "WarTextBox.h"
#include "ArticleParams.h"
#include "TextParams.h"

#include "common/xf/FontManager.h"
#include "common/legacy/ui/Container.h"

class Article : public chr::Container
{
public:
  Article();
  Article(const ArticleParams &params, chr::xf::FontManager &fontManager);

  void addTextBox(const std::u16string &text, const TextParams &params, chr::xf::FontManager &fontManager);

  void drawText(DrawingContext &context);
  void drawWalls(DrawingContext &context);
  void drawTextures(DrawingContext &context);
  void drawOutlines(DrawingContext &context);
};
