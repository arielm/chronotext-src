#pragma once

#include "common/tunnel/ObstacleList.h"

#include "chr/path/FollowablePath2D.h"

class StringData
{
public:
  virtual ~StringData() {}
};

class String
{
public:
  int size;
  float *w;
  float *position;
  float *prevPosition;
  float *a;
  float *dx;
  float *dy;
  float *d;
  float length;
  int id;

  Obstacle *obstacleBegin;
  Obstacle *obstacleEnd;
  
  bool locked;
  float lockPosition;
  float lockDelta;
  int lockId;
  int lockIndex;
  
  bool pushed;
  Obstacle *pushingObstacleBegin;
  Obstacle *pushingObstacleEnd;

  StringData *data;
  
  String(int capacity);
  ~String();
  
  void setPosition(int index, float position, float w, bool fixed);
  void setPositions(float position);
  void updateValues(const chr::path::FollowablePath2D &path);
  void accumulateForces(float ax, float ay, float gravity);
  void verlet(float dt, float friction);
};
