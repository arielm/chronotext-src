#pragma once

#include "common/tunnel/Finger.h"
#include "common/tunnel/IdProvider.h"

#include "chr/path/FollowablePath2D.h"

#include <list>
#include <map>

class CandidatePoint
{
public:
  float x;
  float y;
  float position;
  float distance;
  int setIndex;
  
  bool remove;
  
  CandidatePoint() : remove(false) {}
  
  static bool compare(const CandidatePoint &lhs, const CandidatePoint &rhs)
  {
    return (lhs.distance < rhs.distance);
  }
};

class FingerTrackerDelegate
{
public:
  virtual void fingerAdded(Finger *finger) {}
  virtual void fingerUpdated(Finger *finger) {}
  virtual void fingerRemoved(Finger *finger) {}
};

class FingerTracker
{
  std::map< int, std::list<FingerRef> > fingerMap;
  
  int capacity;
  float fingerRadius;
  const chr::path::FollowablePath2D &path;
  IdProvider &idProvider;
  FingerTrackerDelegate &delegate;

  void addFinger(int index, int id, float position, double timestamp, float distance);
  void updateFinger(int index, int id, float position, double timestamp, float distance);
  void removeFinger(int index, int id);

public:
  FingerTracker(const chr::path::FollowablePath2D &path, IdProvider &idProvider, FingerTrackerDelegate &delegate);
  void setParams(int capacity, float fingerRadius);

  void updateTouch(int index, const glm::vec2 &point, double timestamp);
  void removeTouch(int index, const glm::vec2 &point);

  std::list<FingerRef>& getFingerList(int index);
  void addFinger(int index, float position, double timestamp);
};
