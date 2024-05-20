#include "EditHistory.h"
#include "Branch.h"

using namespace std;

void EditHistory::reset()
{
  last = nullptr;
  list.clear();
}

void EditHistory::update(shared_ptr<Branch> branch) // called each time a character is added to a branch
{
  if (branch != last)
  {
    setLast(branch);
  }
}

void EditHistory::remove(shared_ptr<Branch> branch)
{
  list.erase(std::remove(list.begin(), list.end(), branch), list.end());
}

void EditHistory::setLast(shared_ptr<Branch> branch)
{
  remove(branch);
  list.push_back(branch);
  last = branch;
}
