#pragma once

#include <memory>

class UILock
{
public:
  typedef std::shared_ptr<UILock> Ref;

  bool check(void *caller);
  bool acquire(void *caller);
  bool release(void *caller);

protected:
  void *locker = nullptr;
  bool locked = false;
};
