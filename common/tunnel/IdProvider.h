#pragma once

#include <vector>

class IdProvider
{
public:
  IdProvider() = default;

  IdProvider(int capacity)
  {
    uid.resize(capacity, 0);
  }

  int getUniqueId(int channel)
  {
    return uid[channel]++;
  }

  void reset(int channel)
  {
    uid[channel] = 0;
  }

protected:
  std::vector<int> uid;
};
