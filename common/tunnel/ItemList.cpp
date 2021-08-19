#include "common/tunnel/ItemList.h"

void ItemList::clear()
{
  list.clear();
}

void ItemList::remove(int type, int id)
{
  for (std::list<Item>::iterator it = list.begin(); it != list.end(); ++it)
  {
    Item i = *it;
    if ((i.type == type) && (i.id == id))
    {
      list.erase(it);
      return;
    }
  }
}

void ItemList::add(int type, int edge, int id, float position)
{
  Item i;
  i.type = type;
  i.edge = edge;
  i.id = id;
  i.position = position;

  list.push_back(i);
}

Item* ItemList::find(int type, int id)
{
  for (std::list<Item>::iterator it = list.begin(); it != list.end(); ++it)
  {
    Item i = *it;
    if ((i.type == type) && (i.id == id))
    {
      return &(*it);
    }
  }

  return nullptr;
}
