#include "common/legacy/ui/UILock.h"

using namespace std;

bool UILock::check(void *caller)
{
  return locked && (locker == caller);
}

bool UILock::acquire(void *caller)
{
  if (!locked || (locker == caller))
  {
    locker = caller;
    locked = true;

    return true;
  }

  return false;
}

bool UILock::release(void *caller)
{
  if (locked && (locker == caller))
  {
    locker = nullptr;
    locked = false;

    return true;
  }

  return false;
}
