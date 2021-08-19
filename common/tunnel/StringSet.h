#pragma once

#include "common/tunnel/String.h"
#include "common/tunnel/Pair.h"
#include "common/tunnel/IdProvider.h"
#include "common/tunnel/FingerTracker.h"

#include <vector>
#include <list>

typedef std::shared_ptr<class StringSet> StringSetRef;

class CustomFingerData : public FingerData
{
public:
  bool usedAsLimit;
  
  CustomFingerData() : usedAsLimit(false) {}
  ~CustomFingerData() {}
};

struct SimulationParams
{
  float dt;
  float friction;
  float gravity;
};

class StringSetData
{
public:
  virtual ~StringSetData() {}
};

class StringSet : public FingerTrackerDelegate
{
public:
  int size;
  std::vector<String*> strings;
  FingerTracker fingerTracker;
  IdProvider &idProvider;
  ObstacleList obstacles;
  ItemList itemList;
  std::list<Pair> pairList;
  float spaceWidth;

  chr::path::FollowablePath2D path;
  StringSetData *data;

  StringSet(const chr::path::FollowablePath2D &path, float offset, float spaceWidth, float fingerRadius, IdProvider &idProvider);
  ~StringSet();
  
  void insertString(String *string);
  void addString(String *string);
  void removeString(int index);
  void removeStrings();
  String* getStringById(int id);
  
  void update(float ax, float ay, const SimulationParams &simulationParams);
  void satisfyConstraints();
  void updateObstaclesAndFingers();
  void removeCrossingFingers();
  void defineLocks();
  void unlockRelevantStrings();
  void applyDrag();
  Obstacle* findDragObstacleTowardFirst(Obstacle *obstacle, int index);
  Obstacle* findDragObstacleTowardLast(Obstacle *obstacle, int index);

  void updateTouch(int index, const glm::vec2 &point, double timestamp);
  void removeTouch(int index, const glm::vec2 &point);
  
  void fingerAdded(Finger *finger);
  void fingerUpdated(Finger *finger);
  void fingerRemoved(Finger *finger);

  void updatePairs();
  void preFilterPairs();
  void postFilterPairs();
};
