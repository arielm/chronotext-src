#pragma once

#include "EventListener.h"

class EventCaster
{
public:
  EventCaster() = default;
  virtual ~EventCaster() {};

  void setListener(EventListener *listener)
  {
    this->listener = listener;
  }

  void castEvent(int message)
  {
    if (listener)
    {
      listener->event(this, message);
    }
  }

protected:
  EventListener *listener = nullptr;
};
