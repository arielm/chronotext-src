#pragma once

#include "TextSpan.h"

#include "common/legacy/ui/TextBox.h"

class WarTextBox : public chr::TextBox
{
public:
  WarTextBox() = default;

  void setText(const std::u16string &text) override;

  int updateRangeIds(int id, int start, int end, int hitsPerChar);
  std::vector<std::shared_ptr<TextSpan>> updateSpans();

  void drawText(DrawingContext &context);
  void drawWalls(DrawingContext &context);
  void drawTextures(DrawingContext &context);
  void drawOutlines(DrawingContext &context);

protected:
  std::vector<std::shared_ptr<TextSpan>> spans;
  std::vector<std::shared_ptr<TextSpan>> list;

  chr::xf::FontSequence spanSequence;

  std::vector<std::shared_ptr<TextSpan>> getRangeSpans(int line, float x1, float x2);
  void updateBatch() override;
};
