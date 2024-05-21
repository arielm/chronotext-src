#include "Article.h"

using namespace std;
using namespace chr;
using namespace xf;

static constexpr float PADDING_HORIZ = 24;
static constexpr float PADDING_VERT = 24;
static constexpr float WIDTH = 460 + PADDING_HORIZ * 2;

static constexpr int TITLE = 0;
static constexpr int SUBTITLE = 1;
static constexpr int SOURCE = 2;
static constexpr int BODY = 3;

Article::Article()
{
  setWidth(WIDTH);
  setPadding(PADDING_HORIZ, PADDING_VERT, PADDING_HORIZ, PADDING_VERT);
}

Article::Article(const ArticleParams &params, FontManager &fontManager)
:
Article()
{
  static TextParams TEXT_PARAMS[5];

  TEXT_PARAMS[TITLE].wrap = true;
  TEXT_PARAMS[TITLE].fontFamily = "serif bold";
  TEXT_PARAMS[TITLE].fontSize = 24;
  TEXT_PARAMS[TITLE].marginTop = 15;
  TEXT_PARAMS[TITLE].marginBottom = 15;

  TEXT_PARAMS[SUBTITLE].wrap = true;
  TEXT_PARAMS[SUBTITLE].fontFamily = "serif";
  TEXT_PARAMS[SUBTITLE].fontSize = 20;
  TEXT_PARAMS[SUBTITLE].marginTop = 15;
  TEXT_PARAMS[SUBTITLE].marginBottom = 15;

  TEXT_PARAMS[SOURCE].wrap = true;
  TEXT_PARAMS[SOURCE].fontFamily = "serif bold";
  TEXT_PARAMS[SOURCE].fontSize = 14;
  TEXT_PARAMS[SOURCE].marginTop = 15;
  TEXT_PARAMS[SOURCE].marginBottom = 15;

  TEXT_PARAMS[BODY].wrap = true;
  TEXT_PARAMS[BODY].fontFamily = "serif";
  TEXT_PARAMS[BODY].fontSize = 13;
  TEXT_PARAMS[BODY].lineHeight = 15;
  TEXT_PARAMS[BODY].marginTop = 15;
  TEXT_PARAMS[BODY].marginBottom = 15;

  // ---

  addTextBox(params.titleText, TEXT_PARAMS[TITLE], fontManager);
  addTextBox(params.subtitleText, TEXT_PARAMS[SUBTITLE], fontManager);
  addTextBox(params.sourceText, TEXT_PARAMS[SOURCE], fontManager);
  addTextBox(params.bodyText, TEXT_PARAMS[BODY], fontManager);

  Container::layoutRequest = true;
}

void Article::addTextBox(const u16string &text, const TextParams &params, FontManager &fontManager)
{
  if (!text.empty())
  {
    auto textBox = make_shared<WarTextBox>();
    Container::addComponent(textBox);

    if (params.fontFamily == "serif")
    {
      textBox->setFont(fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties3d()));
    }
    else if (params.fontFamily == "serif bold")
    {
      textBox->setFont(fontManager.getFont(InputSource::resource("Georgia_Bold_64.fnt"), XFont::Properties3d()));
    }

    textBox->setFontSize(params.fontSize);

    textBox->setPadding(params.paddingLeft, params.paddingTop, params.paddingRight, params.paddingBottom);
    textBox->setMargin(params.marginLeft, params.marginTop, params.marginRight, params.marginBottom);

    textBox->wrap = params.wrap;

    textBox->setTextAlign(params.textAlignX, params.textAlignY);
    textBox->setTextColor(params.fontColor);

    textBox->offsetX = params.offsetX;
    textBox->offsetY = params.offsetY;

    if (params.lineHeight > 0)
    {
      textBox->setLineHeight(params.lineHeight);
    }
    else if (params.lineHeightFactor > 0)
    {
      textBox->setLineHeightFactor(params.lineHeightFactor);
    }

    // ---

    textBox->setText(text);
  }
}


void Article::drawText(DrawingContext &context)
{
  for (auto component : components)
  {
    auto textBox = static_pointer_cast<WarTextBox>(component);
    textBox->drawText(context);
  }
}

void Article::drawWalls(DrawingContext &context)
{
  for (auto component : components)
  {
    auto textBox = static_pointer_cast<WarTextBox>(component);
    textBox->drawWalls(context);
  }
}

void Article::drawTextures(DrawingContext &context)
{
  for (auto component : components)
  {
    auto textBox = static_pointer_cast<WarTextBox>(component);
    textBox->drawTextures(context);
  }
}

void Article::drawOutlines(DrawingContext &context)
{
  for (auto component : components)
  {
    auto textBox = static_pointer_cast<WarTextBox>(component);
    textBox->drawOutlines(context);
  }
}
