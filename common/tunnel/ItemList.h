#pragma once

#include <list>

/*
 * AN ITEM IS A THICK AND SOFT LIMIT
 */
class Item
{
public:
  enum
  {
    TYPE_END,
    TYPE_COIN,
  };

  enum
  {
    EDGE_BEGIN,
    EDGE_END,
  };

  int type;
  int edge;
  int id;
  float position;
};

class ItemList
{
public:
  std::list<Item> list;

  void clear();
  void remove(int type, int id);
  void add(int type, int edge, int id, float position);
  
  Item* find(int type, int id);
};
