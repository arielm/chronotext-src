#include "common/tunnel/ObstacleList.h"

#include <limits>

using namespace std;

void ObstacleList::setHead(float position)
{
  Obstacle head;
  head.position = position;
  head.type = Obstacle::TYPE_EDGE;
  head.id = numeric_limits<int>::min();

  list.push_front(head);
}

void ObstacleList::setTail(float position)
{
  Obstacle tail;
  tail.position = position;
  tail.type = Obstacle::TYPE_EDGE;
  tail.id = numeric_limits<int>::max();

  list.push_back(tail);
}

Obstacle* ObstacleList::insert(int type, int id, float position)
{
  remove(type, id);

  Obstacle o;
  o.position = position;
  o.type = type;
  o.id = id;

  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if (position <= it->position)
    {
      list.insert(it, o);

      --it;
      return &(*it);
    }
  }

  return nullptr;
}

Obstacle* ObstacleList::insert(Finger *finger)
{
  remove(Obstacle::TYPE_FINGER, finger->id);

  Obstacle o;
  o.position = finger->position;
  o.type = Obstacle::TYPE_FINGER;
  o.id = finger->id;
  o.finger = finger;

  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if (finger->position <= it->position)
    {
      list.insert(it, o);

      --it;
      return &(*it);
    }
  }

  return nullptr;
}

void ObstacleList::update(int type, int id, float position)
{
  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if ((it->type == type) && (it->id == id))
    {
      it->position = position;
      return;
    }
  }
}

void ObstacleList::update(Finger *finger)
{
  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if ((it->type == Obstacle::TYPE_FINGER) && (it->id == finger->id))
    {
      it->position = finger->position;
      it->finger = finger;
      return;
    }
  }
}

void ObstacleList::remove(int type, int id)
{
  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if ((it->type == type) && (it->id == id))
    {
      list.erase(it);
      return;
    }
  }
}

void ObstacleList::clear()
{
  list.clear();
}


Obstacle* ObstacleList::getHead()
{
  return &(list.front());
}

Obstacle* ObstacleList::getTail()
{
  return &(list.back());
}

Obstacle* ObstacleList::find(int type, int id)
{
  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if ((it->type == type) && (it->id == id))
    {
      return &(*it);
    }
  }

  return nullptr;
}

Obstacle* ObstacleList::findTowardLast(float end)
{
  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if (it->position >= end)
    {
      return &(*it);
    }
  }

  return &(list.back());
}

Obstacle* ObstacleList::findTowardFirst(float begin)
{
  for (std::list<Obstacle>::reverse_iterator it = list.rbegin(); it != list.rend(); ++it)
  {
    if (it->position <= begin)
    {
      return &(*it);
    }
  }

  return &(list.front());
}

Obstacle* ObstacleList::findTowardLast(float end, Obstacle *obstacleToExclude)
{
  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
  if ((&(*it) != obstacleToExclude) && (it->position >= end))
  {
    return &(*it);
  }
  }

  return &(list.back());
}

Obstacle* ObstacleList::findTowardFirst(float begin, Obstacle *obstacleToExclude)
{
  for (std::list<Obstacle>::reverse_iterator it = list.rbegin(); it != list.rend(); ++it)
  {
    if ((&(*it) != obstacleToExclude) && (it->position <= begin))
    {
      return &(*it);
    }
  }

  return &(list.front());
}

void ObstacleList::sortByDistance()
{
  list.sort(Obstacle::compareDistance);
}

void ObstacleList::sortByPosition()
{
  list.sort(Obstacle::comparePosition);

  /*
   * MAKING SURE THAT HEAD AND TAIL
   * ARE PROPERLY ORDERED
   */

  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if ((it->type == Obstacle::TYPE_EDGE) && (it->id < 0))
    {
      list.splice(list.begin(), list, it);
      break;
    }
  }

  for (std::list<Obstacle>::iterator it = list.begin(); it != list.end(); ++it)
  {
    if ((it->type == Obstacle::TYPE_EDGE) && (it->id > 0))
    {
      list.splice(list.end(), list, it);
      break;
    }
  }
}
