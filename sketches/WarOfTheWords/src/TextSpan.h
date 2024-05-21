#pragma once

#include "Missile.h"
#include "DrawingContext.h"

#include "common/xf/Font.h"

class WarTextBox;

static const glm::vec3 PALETTE[8] =
{
  { 1.0f, 0.8f, 0.8f },
  { 1.0f, 1.0f, 0.6f },
  { 1.0f, 0.8f, 0.4f },
  { 1.0f, 1.0f, 0.2f },
  { 0.4f, 1.0f, 1.0f },
  { 0.4f, 0.8f, 0.8f },
  { 0.4f, 1.0f, 0.6f },
  { 0.4f, 0.8f, 0.4f }
};

class TextSpan
{
public:
  int id;
  int start, end;

  int line;
  float x1, y1, x2, y2;

  std::u16string text;
  std::shared_ptr<chr::XFont> font;

  std::vector<std::shared_ptr<TextSpan>> topSpans, bottomSpans;

  // ---

  float t0;

  // ELEVATION
  static constexpr float elevationDurationMax = 4; // SIMILAR TO SearchManager::firstLaunch
  float elevationDuration;
  float elevationMax;
  float elevation;

  // ROTATION
  static constexpr float rotationDuration = 0.6f;
  static constexpr float amplitudeMax = 135 * chr::D2R;
  static constexpr float amplitudeLimited = 90 * chr::D2R;
  float angleTop, angleBottom;
  float rotationDirection;

  // LAUNCH
  static constexpr float launchDuration = 0.1f; // FROM BEGINNING OF ROTATION
  std::shared_ptr<Missile> missile;

  // CLOSING
  static constexpr float closingDuration = 0.1f; // FROM END OF ROTATION

  // COLLAPSING
  float hitsPerChar; // NUMBER OF HITS IT WILL TAKE TO COLLAPSE, PER CHARACTER
  float collapseAmount;
  float collapseElevationStart, collapseElevationEnd;
  float collapseT0;

  bool elevating = false, collapsing = false, rotating = false, launching = false;
  bool opened = false, burried = false;

  float textureOffsetX, textureOffsetY;
  static constexpr float textureZoom = 0.4f;
  static constexpr float textureOpacity = 0.2f;

  TextSpan() = default;
  TextSpan(const WarTextBox &parent, int id, int start, int end, int hitsPerChar);

  void reset();
  void update(int line, float x1, float y1, float x2, float y2);
  void update(std::vector<std::shared_ptr<TextSpan>> topSpans, std::vector<std::shared_ptr<TextSpan>> bottomSpans);
  void update(size_t minLength, size_t maxLength);
  void update(float offsetX, float offsetY);

  chr::math::Rectf getBounds();
  bool isLimited(std::vector<std::shared_ptr<TextSpan>> spans, float limit);

  void run(float elapsed);
  void draw(DrawingContext &context, int pass);

  void elevate(float elapsed);
  void collapse(float t);
  void open(float t);
  void close(float t);
  void launch(float t, std::shared_ptr<Missile> missile);

protected:
  void drawPass0(DrawingContext &context);
  void drawPass1(DrawingContext &context);
  void drawPass2(DrawingContext &context);
  void drawPass3(DrawingContext &context);
  void drawPass4(DrawingContext &context);
  void drawPass5(DrawingContext &context);

  void drawText(const chr::gl::Matrix &matrix, float x, float y);
};
