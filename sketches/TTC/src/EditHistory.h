#pragma once

#include <vector>
#include <memory>

class Branch;

class EditHistory
{
public:
  std::vector<std::shared_ptr<Branch>> list;

  void reset();
  void update(std::shared_ptr<Branch> branch); // called each time a character is added to a branch
  void remove(std::shared_ptr<Branch> branch);
  void setLast(std::shared_ptr<Branch> branch);

protected:
  std::shared_ptr<Branch> last;
};
