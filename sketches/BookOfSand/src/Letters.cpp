#include "Letters.h"

using namespace std;
using namespace chr;

static constexpr float dt = 0.1f; // affects the overall speed of the physics...
static constexpr float gravity = 1.0f;
static constexpr float friction = 0.2f; // a value between 0 and 1

static constexpr int NUM_ITERATIONS = 200;

Letters::Letters(CompositePath &path)
:
path(path)
{}

void Letters::setText(shared_ptr<chr::XFont> font, const u16string &text, float pos, float vel)
{
  this->font = font;
  this->text = text;
  n = text.size();

  x.resize(n);
  firstComputation.resize(n);
  theta.resize(n);
  position.resize(n);
  a.resize(n);
  v.resize(n);
  D.resize(n);
  w.resize(n);

  float offset = 0;
  for (int i = 0; i < n; i++)
  {
    position[i] = pos + offset;
    v[i] = vel;
    w[i] = font->getCharAdvance(text[i]);
    firstComputation[i] = false;

    if (i > 0) // D[0] is never used...
    {
      D[i] = position[i] - position[i - 1];
    }

    offset += w[i];
  }
}

void Letters::update()
{
  for (int i = 0; i < n; i++)
  {
    if (!firstComputation[i])
    {
      firstComputation[i] = true;

      auto value = path.offsetToValue(position[i]);
      x[i] = value.position.x;
      theta[i] = atan2f(value.forward.y, value.forward.x);
    }
    position[i] = path.getOffset(x[i]); // ensuring that positions are not affected by the length of the path
  }

  accumulateForces();
  verlet();
  satisfyConstraints();

  for (int i = 0; i < n; i++)
  {
    auto value = path.offsetToValue(position[i]);
    x[i] = value.position.x;
    theta[i] = atan2f(value.forward.y, value.forward.x);
  }
}

void Letters::draw()
{
  Matrix matrix;

  float stringLength = position[n - 1] + w[n - 1] - position[0];
  float offset = position[0] + (stringLength - font->getStringAdvance(text)) / 2;

  for (int i = 0; i < n; i++)
  {
    auto glyphIndex = font->getGlyphIndex(text[i]);

    if (glyphIndex > -1)
    {
      auto value = path.offsetToValue(offset + w[i] * 0.5f, w[i]);
      float angle = atan2f(value.forward.y, value.forward.x);

      matrix
        .setTranslate(value.position)
        .rotateZ(angle);

      font->addGlyph(matrix, glyphIndex, -w[i] * 0.5f, font->getOffsetY(XFont::ALIGN_MIDDLE));
    }

    offset += w[i];
  }
}

void Letters::accumulateForces()
{
  for (int i = 0; i < n; i++)
  {
    a[i] = gravity * sinf(theta[i]);
    a[i] -= v[i] * friction;
  }
}

void Letters::verlet()
{
  for (int i = 0; i < n; i++)
  {
    float tmp = position[i];
    position[i] += v[i] + a[i] * dt;
    v[i] = position[i] - tmp;
  }
}

void Letters::satisfyConstraints()
{
  for (int j = 0; j < NUM_ITERATIONS; j++)
  {
    for (int i = 1; i < n; i++)
    {
      float delta = (position[i] - position[i - 1] - D[i]) * 0.5f;
      position[i - 1] += delta;
      position[i] -= delta;
    }
  }
}
