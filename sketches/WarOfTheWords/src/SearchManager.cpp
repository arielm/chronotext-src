#include "SearchManager.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;

static constexpr int firstLaunch = 4;
static constexpr float launchingDelay = 0.2f; // XXX
static constexpr int maxActiveMissiles = 100;
static constexpr float hitPadding = 0.1f;

SearchManager::SearchManager()
:
random(12345678)
{}

void SearchManager::reset()
{
  step = 0;
  t0 = 0;
  lastMissileT = 0;

  missileManager.reset();

  for (auto span : spans)
  {
    span->reset();
  }
}

void SearchManager::run(float elapsed)
{
  if (t0 == 0)
  {
    t0 = elapsed;
  }

  float t = elapsed - t0;

  switch (step)
  {
    case 0:
    {
      for (auto span : spans)
      {
        span->elevate(elapsed);
      }

      step = 1;
      t0 = elapsed;
    }
    break;

    case 1:
    {
      if (t >= firstLaunch)
      {
        step = 2;
        t0 = elapsed;
      }
    }
    break;

    case 2 :
    {
      if ((missileManager.getNumActiveMissiles() < maxActiveMissiles) && (elapsed - lastMissileT >= launchingDelay))
      {
        auto candidates = getCandidates();

        if (!candidates.empty())
        {
          auto candidate = candidates[random.nextInt(candidates.size())];
          auto TargetSpans = candidate.second;

          auto span1 = candidate.first;
          auto span2 = TargetSpans[random.nextInt(TargetSpans.size())];

          launch(elapsed, span1, span2);
          lastMissileT = elapsed;
        }
      }
    }
    break;
  }

  if (step >= 2)
  {
    missileManager.run(elapsed);
  }

  // ---

  for (auto span : spans)
  {
    span->run(elapsed);
  }
}

void SearchManager::search()
{
  article->layout();

  for (const auto &term : candidateTerms)
  {
    if (search(term))
    {
      terms.push_back(term);
    }
  }
}

bool SearchManager::search(const Term &term)
{
  bool found = false;

  for (auto component : article->components)
  {
    auto searchable = static_pointer_cast<WarTextBox>(component);
    found |= search(searchable, term);
  }

  return found;
}

bool SearchManager::search(shared_ptr<WarTextBox> searchable, const Term &term)
{
  int found = 0;
  size_t start = 0;

  auto termSize = term.text.size();

  while (true)
  {
    start = searchable->text.find(term.text.data(), start, termSize);

    if (start == string::npos)
    {
      break;
    }
    else
    {
      found += searchable->updateRangeIds(term.id, start, start + termSize, term.hitsPerChar);
      start += termSize;
    }
  }

  return (found > 0);
}

void SearchManager::refresh()
{
  minTermLength = INT_MAX;
  maxTermLength = 0;

  for (const auto &term : terms)
  {
    minTermLength = min(minTermLength, term.text.size());
    maxTermLength = max(maxTermLength, term.text.size());
  }

  // ---

  for (auto component : article->components)
  {
    auto searchable = static_pointer_cast<WarTextBox>(component);
    auto list1 = searchable->updateSpans();
    spans.insert(spans.end(), list1.begin(), list1.end());
  }

  // ---

  for (auto span : spans)
  {
    auto key = span->id;
    auto found = spansById.find(key);

    if (found != spansById.end())
    {
      found->second.push_back(span);
    }
    else
    {
      spansById.emplace(key, vector<shared_ptr<TextSpan>>()).first->second.push_back(span);
    }

    span->update(minTermLength, maxTermLength);
    span->update(random.nextFloat(), random.nextFloat());
  }
}

void SearchManager::addCandidateTerm(const Term &term)
{
  candidateTerms.push_back(term);
}

vector<Term>& SearchManager::getTerms()
{
  return terms;
}

vector<shared_ptr<TextSpan>>& SearchManager::getSpans()
{
  return spans;
}

vector<shared_ptr<TextSpan>> SearchManager::getTargetSpansForTerm(const Term &term1)
{
  vector<shared_ptr<TextSpan>> targetSpans;

  for (const auto &term2 : term1.enemies)
  {
    for (auto span : spansById[term2.id])
    {
      if (!span->burried)
      {
        targetSpans.push_back(span);
      }
    }
  }

  return targetSpans;
}

vector<pair<shared_ptr<TextSpan>, vector<shared_ptr<TextSpan>>>> SearchManager::getCandidates()
{
  vector<pair<shared_ptr<TextSpan>, vector<shared_ptr<TextSpan>>>> candidates;

  for (const auto &term1 : terms)
  {
    auto targetSpans = getTargetSpansForTerm(term1);

    for (auto span : spansById[term1.id])
    {
      if (!span->launching && !targetSpans.empty())
      {
        candidates.emplace_back(span, targetSpans);
      }
    }
  }

  return candidates;
}

void SearchManager::launch(float elapsed, shared_ptr<TextSpan> span1, shared_ptr<TextSpan> span2)
{
  auto bounds1 = span1->getBounds();
  glm::vec2 p1 = bounds1.x1y1() + bounds1.size() * 0.5f;

  auto bounds2 = span2->getBounds();
  glm::vec2 p2 = bounds2.x1y1() + bounds2.size() * (hitPadding + glm::vec2(random.nextFloat(), random.nextFloat()) * (1 - hitPadding * 2));

  span1->launch(elapsed, missileManager.createMissile(random, elapsed, p1.x, p1.y, 0, Target(span2, p2.x, p2.y)));
}
