#pragma once

#include <string>

struct Term
{
  std::string text;
  int frequency;

  Term(const std::string &text)
  :
  text(text),
  frequency(1)
  {}
};
