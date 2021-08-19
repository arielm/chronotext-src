#pragma once

#include "common/tunnel/Finger.h"

#include <list>
#include <map>

/*
 * AN OBSTACLE IS A THIN AND HARD LIMIT
 */
class Obstacle
{
public:
  enum
  {
    TYPE_EDGE,
    TYPE_FINGER
  };

  int type;
  int id;
  float position;
  
  int lockedStringId;
  float lockPosition;
  
  bool compressing;
  float compressionDirection;
  
  std::map<int,float> deltas;
  bool remove;
  
  Finger *finger;
  
  Obstacle() : lockedStringId(-1), compressing(false), remove(false), finger(nullptr) {}

  static bool comparePosition(const Obstacle &lhs, const Obstacle &rhs)
  {
    return (lhs.position < rhs.position);
  }
  
  static bool compareDistance(const Obstacle &lhs, const Obstacle &rhs)
  {
    if ((lhs.type != TYPE_FINGER) || (rhs.type != TYPE_FINGER))
    {
      return false;
    }

    return (lhs.finger->distance < rhs.finger->distance);
  }
  
  static bool shouldBeRemoved(const Obstacle &obstacle)
  {
    return obstacle.remove;
  }
};

class ObstacleList
{
public:
  std::list<Obstacle> list;
  
  void setHead(float position);
  void setTail(float position);

  Obstacle* insert(int type, int id, float position);
  Obstacle* insert(Finger *finger);
  
  void update(int type, int id, float position);
  void update(Finger *finger);
  
  void remove(int type, int id);
  void clear();

  Obstacle* getHead();
  Obstacle* getTail();
  
  Obstacle* find(int type, int id);

  Obstacle* findTowardLast(float end);
  Obstacle* findTowardFirst(float begin);
  
  Obstacle* findTowardLast(float end, Obstacle *obstacleToExclude);
  Obstacle* findTowardFirst(float begin, Obstacle *obstacleToExclude);
  
  void sortByDistance();
  void sortByPosition();
};
