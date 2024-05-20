#include "NavHistory.h"
#include "Branch.h"

using namespace std;

void NavHistory::reset()
{
  list.clear();
}

void NavHistory::add(shared_ptr<Branch> branch)
{
  remove(branch);
  list.push_back(branch);
}

void NavHistory::remove(shared_ptr<Branch> branch)
{
  list.erase(std::remove(list.begin(), list.end(), branch), list.end());
}

shared_ptr<Branch> NavHistory::back()
{
  if (!list.empty())
  {
    auto last = list.back(); list.pop_back();
    list.insert(list.begin(), last);

    last = list.back(); list.pop_back(); // XXX
    return last;
  }

  return nullptr;
}

shared_ptr<Branch> NavHistory::forward()
{
  if (!list.empty())
  {
    return list.front();
  }

  return nullptr;
}
