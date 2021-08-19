#include "common/tunnel/StringSet.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace path;

static constexpr int NUM_ITERATIONS = 200;
static constexpr int FINGER_CAPACITY = 10;

StringSet::StringSet(const FollowablePath2D &path, float offset, float spaceWidth, float fingerRadius, IdProvider &idProvider)
:
path(path),
spaceWidth(spaceWidth),
idProvider(idProvider),
size(0),
data(nullptr),
fingerTracker(this->path, idProvider, *this)
{
  fingerTracker.setParams(FINGER_CAPACITY, fingerRadius);

  obstacles.setHead(-offset);
  obstacles.setTail(path.getLength() + offset);
}

StringSet::~StringSet()
{
  LOGD << "StringSet DELETED" << endl;
  
  for (vector<String*>::iterator it = strings.begin(); it != strings.end(); ++it)
  {
    delete *it;
  }
  
  if (data)
  {
    delete data;
  }
}

void StringSet::insertString(String *string)
{
  strings.insert(strings.begin(), string);
  
  size++;
  string->id = idProvider.getUniqueId(1);
  string->updateValues(path);
}

void StringSet::addString(String *string)
{
  strings.push_back(string);
  
  size++;
  string->id = idProvider.getUniqueId(1);
  string->updateValues(path);
}

void StringSet::removeString(int index)
{
  delete strings[index];
  strings.erase(strings.begin() + index);
  size--;
}

void StringSet::removeStrings()
{
  for (vector<String*>::iterator it = strings.begin(); it != strings.end(); ++it)
  {
    delete *it;
  }
  
  strings.clear();
  size = 0;
}

String* StringSet::getStringById(int id)
{
  for (int i = 0; i < size; i++)
  {
    if (strings[i]->id == id)
    {
      return &*strings[i];
    }
  }
  
  return nullptr;
}

void StringSet::update(float ax, float ay, const SimulationParams &simulationParams)
{
  updatePairs();
  preFilterPairs();
  
  // ---
  
  updateObstaclesAndFingers();
  
  for (int i = 0; i < size; i++)
  {
    String *string = strings[i];
    
    string->accumulateForces(ax, ay, simulationParams.gravity);
    string->verlet(simulationParams.dt, simulationParams.friction);
  }
  
  satisfyConstraints();
  
  // ---
  
  updatePairs();
  postFilterPairs();
  
  // ---
  
  for (int i = 0; i < size; i++)
  {
    strings[i]->updateValues(path);
  }
}

void StringSet::satisfyConstraints()
{
  for (int i = 0; i < size; i++)
  {
    String *string = strings[i];
    
    string->pushed = false;
    string->pushingObstacleBegin = nullptr;
    string->pushingObstacleEnd = nullptr;
  }
  
  for (int i = 0; i < NUM_ITERATIONS; i++)
  {
    /*
     * KEEPING PROPER SPACING BETWEEN STRINGS
     */
    
    for (int j = 1; j < size; j++)
    {
      /*
       * TODO: OPTIMIZE BY PRECALCULATING r0 AND r1 OUTSIDE OF THE LOOP
       */
      
      String *s0 = strings[j];
      String *s1 = strings[j - 1];
      
      float delta = (s0->position[0] - s1->position[0] - s1->length - spaceWidth);
      if (delta < 0)
      {
        float r0;
        float r1;
        
        if (s0->length < s1->length)
        {
          float ratio = s0->length / (s0->length + s1->length);
          r0 = 1 - ratio;
          r1 = ratio;
        }
        else
        {
          float ratio = s1->length / (s0->length + s1->length);
          r0 = ratio;
          r1 = 1 - ratio;
        }
        
        s0->position[0] -= delta * r0;
        s1->position[0] += delta * r1;
        
        if (s0->pushed)
        {
          s1->pushed = true;
        }
        if (s1->pushed)
        {
          s0->pushed = true;
        }
      }
    }
    
    /*
     * KEEPING INNER STRING SPACING
     */
    
    for (int j = 0; j < size; j++)
    {
      String *string = strings[j];
      
      for (int k = 1; k < string->size; k++)
      {
        float delta = (string->position[k] - string->position[k - 1] - string->d[k]) * 0.5f;
        string->position[k - 1] += delta;
        string->position[k] -= delta;
      }
    }
    
    /*
     * ENFORCING LIMITS DEFINED BY OBSTACLES
     */
    
    for (int j = 0; j < size; j++)
    {
      String *string = strings[j];
      
      if (!string->locked)
      {
        float limitBegin = string->obstacleBegin->position;
        float delta = string->position[0] - limitBegin;
        
        if (delta < 0)
        {
          if (!string->obstacleBegin->compressing)
          {
            string->pushed = true;
            string->pushingObstacleBegin = string->obstacleBegin;
            
            string->position[0] = limitBegin - delta * 0.5f; // XXX: 50% ELASTICITY, BEWARE OF MORE
          }
        }
        
        // ---
        
        float limitEnd = string->obstacleEnd->position;
        delta = string->position[0] + string->length - limitEnd;
        
        if (delta > 0)
        {
          if (!string->obstacleEnd->compressing )
          {
            string->pushed = true;
            string->pushingObstacleEnd = string->obstacleEnd;
            
            string->position[0] = limitEnd - string->length - delta * 0.5f; // XXX: 50% ELASTICITY, BEWARE OF MORE
          }
        }
      }
    }
    
    /*
     * APPLYING FINGER DRAG
     */
    
    for (int j = 0; j < size; j++)
    {
      String *string = strings[j];
      
      if (string->locked)
      {
        string->position[0] = string->lockPosition;
      }
    }
  }
}

void StringSet::updateObstaclesAndFingers()
{
  /*
   * DEFINING OBSTACLES
   */
  
  for (int i = 0; i < FINGER_CAPACITY; i++)
  {
    auto &fingerList = fingerTracker.getFingerList(i);
    
    for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
    {
      FingerRef finger = *it;
      CustomFingerData *data = (CustomFingerData*)finger->data;
      
      if (!data->usedAsLimit)
      {
        data->usedAsLimit = true;
        obstacles.insert(&*finger);
      }
    }
  }
  
  // ---
  
  /*
   * IN ORDER TO AVOID "PASS-THROUGH" SITUATIONS:
   * IF TWO FINGERS CROSS ONE-ANOTHER
   * WE REMOVE THE MOST "RECENT" ONE
   */
  
  removeCrossingFingers();
  
  /*
   * BINDING OBSTACLES WITH STRINGS
   */
  
  for (int i = 0; i < size; i++)
  {
    String *string = strings[i];
    
    string->obstacleBegin = obstacles.findTowardFirst(string->position[0]);
    string->obstacleEnd = obstacles.findTowardLast(string->position[0] + string->length);
  }
  
  // ---
  
  /*
   * HANDLING LOCKS
   */
  
  for (int i = 0; i < size; i++)
  {
    strings[i]->locked = false;
  }
  
  /*
   * SINCE A STRING CAN BE LOCKED BY ONLY
   * ONE TOUCH AT THE TIME, IT IS NECESSARY TO
   * PICK THE BEST (I.E. THE CLOSEST) CANDIDATE
   * WHENEVER THERE IS MORE THAN ONE
   */
  obstacles.sortByDistance();
  
  defineLocks();
  unlockRelevantStrings();
  
  /*
   * BRINGING BACK obstacles TO THE NORMAL SORTING ORDER
   */
  obstacles.sortByPosition();
  
  /*
   * LIMITING FINGER-DRAG WHENEVER THERE IS NO
   * SPACE-LEFT TOWARDS THE NEXT OBSTACLE
   *
   * XXX: SHOULD WE SET "COMPRESSED STATE"
   * ON STRINGS WHEN RELEVANT?
   */
  
  applyDrag();
}

void StringSet::removeCrossingFingers()
{
  for (list<Obstacle>::iterator it1 = obstacles.list.begin(); it1 != obstacles.list.end(); ++it1)
  {
    if (it1->type == Obstacle::TYPE_FINGER)
    {
      for (list<Obstacle>::iterator it2 = obstacles.list.begin(); it2 != obstacles.list.end(); ++it2)
      {
        if ((&(*it1) != &(*it2)) && (it2->type == Obstacle::TYPE_FINGER))
        {
          float delta = it1->position - it2->position;
          
          if (it1->deltas.count(it2->id))
          {
            if (it1->deltas[it2->id] * delta < 0)
            {
              if (it1->id > it2->id)
              {
                it1->remove = true;
                LOGD << "CROSSING-FINGER REMOVED: " << it1->id << endl;
              }
            }
          }
          
          it1->deltas[it2->id] = delta;
        }
      }
    }
  }
  
  obstacles.list.remove_if(Obstacle::shouldBeRemoved);
}

void StringSet::defineLocks()
{
  for (list<Obstacle>::iterator it = obstacles.list.begin(); it != obstacles.list.end(); ++it)
  {
    if (it->type == Obstacle::TYPE_FINGER)
    {
      for (int j = 0; j < size; j++)
      {
        String *string = strings[j];
        int stringId = string->id;
        
        /*
         * XXX: SHOULD WE USE THE "SEPARATING AXIS" METHOD TO
         * AVOID SHORT STRINGS TO BE MISSED BY FAST MOVING FINGERS?
         */
        bool inside = (it->position > string->position[0]) && (it->position < string->position[0] + string->length);
        
        if (!inside)
        {
          if (it->lockedStringId == stringId)
          {
            float delta = it->position - it->lockPosition;
            
            if (!it->compressing)
            {
              it->compressionDirection = delta;
              LOGD << "COMPRESSING: " << j << endl;
            }
            else
            {
              /*
               * RELEASING THE LOCK IF DIRECTION
               * IS CHANGING WHILE COMPRESSING
               *
               * THEN, IMMEDIATELY CHECKING FOR
               * A NEW LOCK SITUATION
               *
               * WE CAN'T AFFORD BEING UNLOCKED FOR
               * EVEN ONE FRAME IF WE DON'T WANT TO
               * MISS SHORT STRINGS WHEN MOVING FAST
               */
              if (delta * it->compressionDirection < 0)
              {
                unlockRelevantStrings();
                defineLocks();
                return;
              }
            }
            
            string->locked = true;
            string->lockDelta = delta;
            
            it->lockPosition = it->position;
            it->compressing = true;
          }
        }
        else
        {
          if (it->lockedStringId == stringId)
          {
            /*
             * THE STRING KEEPS BEING
             * DRAGGED BY THE FINGER
             */
            
            string->locked = true;
            string->lockDelta = it->position - it->lockPosition;
            
            it->lockPosition = it->position;
          }
          else if (!it->compressing && (it->lockedStringId == -1))
          {
            /*
             * ENSURING THAT NO STRING IS LOCKED BY MORE THAN ONE FINGER
             */
            if (string->lockId == -1)
            {
              /*
               * THE STRING STARTS BEING
               * DRAGGED BY THE FINGER
               */
              
              string->locked = true;
              string->lockId = it->id;
              string->lockIndex = it->finger->index;
              string->lockPosition = string->position[0];
              string->lockDelta = 0;
              
              it->lockPosition = it->position;
              it->lockedStringId= stringId;
              
              LOGD << "STRING LOCKED: " << stringId << " BY FINGER: " << it->id << endl;
            }
          }
        }
      }
    }
  }
}

void StringSet::unlockRelevantStrings()
{
  for (int i = 0; i < size; i++)
  {
    String *string = strings[i];
    
    if (!string->locked && (string->lockId != -1))
    {
      Obstacle *obstacle = obstacles.find(Obstacle::TYPE_FINGER, string->lockId);
      if (obstacle != nullptr)
      {
        obstacle->lockedStringId = -1;
        obstacle->compressing = false;
      }
      
      string->lockId = -1;
      LOGD << "STRING UNLOCKED: " << string->id << endl;
    }
  }
}

void StringSet::applyDrag()
{
  for (int i = 0; i < size; i++)
  {
    String *string = strings[i];
    
    if (string->locked)
    {
      Obstacle *obstacle = obstacles.find(Obstacle::TYPE_FINGER, string->lockId);

      if (obstacle)
      {
        float delta = string->lockDelta;

        if (delta != 0)
        {
          float position = string->lockPosition + delta;
          
          if (delta < 0)
          {
            float limitBegin = findDragObstacleTowardFirst(obstacle, string->lockIndex)->position;
            float spaceBegin = 0;
            
            for (int k = i - 1; k >= 0; k--)
            {
              if (strings[k]->locked)
              {
                limitBegin = strings[k]->position[0] + strings[k]->length + spaceWidth;
                break;
              }
              if (strings[k]->obstacleBegin->position < limitBegin)
              {
                break;
              }
              
              spaceBegin += strings[k]->length + spaceWidth;
            }
            
            if (position - spaceBegin <= limitBegin)
            {
              string->lockPosition = limitBegin + spaceBegin;
              continue;
            }
          }
          else if (delta > 0)
          {
            float limitEnd = findDragObstacleTowardLast(obstacle, string->lockIndex)->position;
            float spaceEnd = 0;
            
            for (int k = i + 1; k < size; k++)
            {
              if (strings[k]->locked)
              {
                limitEnd = strings[k]->position[0] - spaceWidth;
                break;
              }
              if (strings[k]->obstacleEnd->position > limitEnd)
              {
                break;
              }
              
              spaceEnd += strings[k]->length + spaceWidth;
            }
            
            if (position + string->length + spaceEnd >= limitEnd)
            {
              string->lockPosition = limitEnd - string->length - spaceEnd;
              continue;
            }
          }
          
          string->lockPosition = position;
        }
      }
    }
  }
}

/*
 * IN ORDER TO AVOID "PASS-THROUGH" SITUATIONS:
 * ONLY ONE CANDIDATE PER TOUCH CAN ACT AS A DRAG-LIMIT
 */

Obstacle* StringSet::findDragObstacleTowardFirst(Obstacle *obstacle, int index)
{
  while (true)
  {
    Obstacle *first = obstacles.findTowardFirst(obstacle->position, obstacle);
    if ((first->type == Obstacle::TYPE_EDGE) || (first->finger->index != index))
    {
      return first;
    }
    obstacle = first;
  }
  
  return nullptr;
}

Obstacle* StringSet::findDragObstacleTowardLast(Obstacle *obstacle, int index)
{
  while (true)
  {
    Obstacle *last = obstacles.findTowardLast(obstacle->position, obstacle);
    if ((last->type == Obstacle::TYPE_EDGE) || (last->finger->index != index))
    {
      return last;
    }
    obstacle = last;
  }
  
  return nullptr;
}

// ======================================== TOUCH ========================================

void StringSet::updateTouch(int index, const glm::vec2 &point, double timestamp)
{
  fingerTracker.updateTouch(index, point, timestamp);
}

void StringSet::removeTouch(int index, const glm::vec2 &point)
{
  fingerTracker.removeTouch(index, point);
}

void StringSet::fingerAdded(Finger *finger)
{
  finger->data = new CustomFingerData;
}

void StringSet::fingerUpdated(Finger *finger)
{
  if (((CustomFingerData*)finger->data)->usedAsLimit)
  {
    obstacles.update(Obstacle::TYPE_FINGER, finger->id, finger->position);
  }
}

void StringSet::fingerRemoved(Finger *finger)
{
  for (int i = 0; i < size; i++)
  {
    String *string = strings[i];
    
    if ((string->obstacleBegin != nullptr) && (string->obstacleBegin->type == Obstacle::TYPE_FINGER) && (string->obstacleBegin->id == finger->id))
    {
      string->obstacleBegin = nullptr;
    }
    
    if ((string->obstacleEnd != nullptr) && (string->obstacleEnd->type == Obstacle::TYPE_FINGER) && (string->obstacleEnd->id == finger->id))
    {
      string->obstacleEnd = nullptr;
    }
  }
  
  if (((CustomFingerData*)finger->data)->usedAsLimit)
  {
    obstacles.remove(Obstacle::TYPE_FINGER, finger->id);
  }
}

// ======================================== LIMITS ========================================

void StringSet::updatePairs()
{
  for (list<Pair>::iterator it = pairList.begin(); it != pairList.end(); ++it)
  {
    int stringId = it->target.stringId;
    int index = it->target.index;
    
    String *string = getStringById(stringId);
    float position = string->position[index];
    
    if (it->item.edge == Item::EDGE_BEGIN)
    {
      it->delta = it->item.position - (position + string->w[index]);
    }
    else
    {
      it->delta = position - it->item.position;
    }
  }
}

static bool preFilter(const Pair &p)
{
  return (p.delta < 0);
}

static bool postFilter(const Pair &p)
{
  return (p.delta > 0);
}

/*
 * REMOVING PAIRS WITH delta < 0
 */
void StringSet::preFilterPairs()
{
  pairList.remove_if(preFilter);
}

/*
 * REMOVING PAIRS WITH delta > 0
 */
void StringSet::postFilterPairs()
{
  pairList.remove_if(postFilter);
}
