#pragma once

#include <vector>
#include <memory>

class Branch;

class NavHistory
{
public:
  std::vector<std::shared_ptr<Branch>> list;

  void reset();
  void add(std::shared_ptr<Branch> branch);
  void remove(std::shared_ptr<Branch> branch);
  std::shared_ptr<Branch> back();
  std::shared_ptr<Branch> forward();
};
