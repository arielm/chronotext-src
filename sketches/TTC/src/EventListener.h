#pragma once

class EventCaster;

class EventListener
{
public:
  EventListener() = default;
  virtual ~EventListener() {};

  virtual void event(EventCaster *source, int message) = 0;
};
