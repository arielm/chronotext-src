#pragma once

#include "Article.h"
#include "Term.h"
#include "MissileManager.h"

#include "chr/Random.h"

#include <unordered_map>

class SearchManager
{
public:
  std::shared_ptr<Article> article;
  MissileManager missileManager;

  SearchManager();

  void reset();
  void run(float elapsed);

  void search();
  bool search(const Term &term);
  bool search(std::shared_ptr<WarTextBox> searchable, const Term &term);
  void refresh();
  void addCandidateTerm(const Term &term);

  std::vector<Term>& getTerms();
  std::vector<std::shared_ptr<TextSpan>>& getSpans();

protected:
  chr::Random random;
  size_t minTermLength, maxTermLength;

  int step = 0;
  int t0 = 0;
  int lastMissileT = 0;

  std::vector<std::shared_ptr<TextSpan>> spans;
  std::unordered_map<int, std::vector<std::shared_ptr<TextSpan>>> spansById;

  std::vector<Term> candidateTerms;
  std::vector<Term> terms;

  std::vector<std::shared_ptr<TextSpan>> getTargetSpansForTerm(const Term &term1);
  std::vector<std::pair<std::shared_ptr<TextSpan>, std::vector<std::shared_ptr<TextSpan>>>> getCandidates();
  void launch(float elapsed, std::shared_ptr<TextSpan> span1, std::shared_ptr<TextSpan> span2);
};
