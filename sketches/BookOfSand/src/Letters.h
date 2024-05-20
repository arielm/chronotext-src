#pragma once

#include "CompositePath.h"

#include "common/xf/Font.h"

class Letters
{
public:
  int n;
  std::vector<float> position;
  CompositePath &path;

  Letters(CompositePath &path);

  void setText(std::shared_ptr<chr::XFont> font, const std::u16string &text, float pos, float vel);
  void update();
  void draw();

protected:
  std::shared_ptr<chr::XFont> font;

  std::u16string text;
  std::vector<float> x, theta;
  std::vector<bool> firstComputation;
  std::vector<float> a;
  std::vector<float> v;
  std::vector<float> D;
  std::vector<float> w;

  void accumulateForces();
  void verlet();
  void satisfyConstraints();
};
