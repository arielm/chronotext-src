#pragma once

#include <string>
#include <set>

struct Term
{
  std::u16string text;
  int id;
  int hitsPerChar;

  std::vector<Term> enemies;

  Term() = default;

  Term(int group, int hitsPerChar, const std::u16string &text)
  :
  id(group - 1),
  hitsPerChar(hitsPerChar),
  text(text)
  {}

  void addEnemy(const Term &term)
  {
    for (const auto &enemy : enemies)
    {
      if ((term.id == enemy.id) && (term.hitsPerChar == enemy.hitsPerChar) && (term.text == enemy.text))
      {
        return;
      }
    }

    enemies.push_back(term);
  }
};
