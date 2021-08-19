#include "common/tunnel/FingerTracker.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace chr;
using namespace path;

FingerTracker::FingerTracker(const FollowablePath2D &path, IdProvider &idProvider, FingerTrackerDelegate &delegate)
:
path(path),
idProvider(idProvider),
delegate(delegate)
{}

static bool fingerIsUnpaired(const FingerRef finger)
{
  if ((finger->updateCount > 0) && (!finger->paired))
  {
    LOGD << "NOT PAIRED: "
      << finger->id
      << " [POS: "
      << finger->position
      << "]" << endl;
  }

  return ((finger->updateCount > 0) && (!finger->paired));
}

void FingerTracker::setParams(int capacity, float fingerRadius)
{
  this->capacity = capacity;
  this->fingerRadius = fingerRadius;

  for (int i = 0; i < capacity; i++)
  {
    fingerMap.insert(make_pair(i, list<FingerRef>()));
  }
}

void FingerTracker::updateTouch(int index, const glm::vec2 &point, double timestamp)
{
  if (index < capacity)
  {
    vector<CandidatePoint> candidates1;
    
    for (int j = 0, end = path.size() - 1; j < end; j++)
    {
      const FollowablePath2D::ClosePoint closestPoint = path.closestPointFromSegment(point, j);
      
      /*
       * SENSING QUITE FAR FROM THE CURVE, 
       * TO COMPENSATE FOR FINGER UNPRECISION
       */
      if (closestPoint.distance <= fingerRadius)
      {
        CandidatePoint candidate;
        candidate.x = closestPoint.position.x; // XXX
        candidate.y = closestPoint.position.y; // XXX
        candidate.position = closestPoint.offset; // XXX
        candidate.distance = closestPoint.distance;
        
        candidates1.push_back(candidate);
      }
    }
    
    /*
     * WHEN DETECTING CANDIDATES ON A PATH:
     * SORT THEM BY THE MINIMUM DISTANCE TO THE FINGER
     * THEN, ALWAYS SELECT THE FIRST ONE
     * REGARDING THE OTHERS: SELECT THE ONES WHICH ARE >THRESHOLD (PATH-POSITION WISE) FROM THE OTHER SELECTED POINTS
     */
    
    sort(candidates1.begin(), candidates1.end(), CandidatePoint::compare);
    
    vector<int> positions;
    vector<CandidatePoint> candidates2;
    
    for (vector<CandidatePoint>::iterator it = candidates1.begin(); it != candidates1.end(); ++it)
    {
      bool skip = false;
      for (int i = 0; i < positions.size(); i++)
      {
        if (fabsf(it->position - positions[i]) < fingerRadius) // XXX
        {
          skip = true;
          break;
        }
      }
      if (skip) continue;
      
      positions.push_back(it->position);
      candidates2.push_back(*it);
    }
    
    /*
     * DISCARDING POINTS WHICH ARE TOO FAR FROM
     * THE CENTER *WHENEVER* ANOTHER POINT IN THE VICINITY
     * (PATH-POSITION WISE) IS CLOSER
     */
    
    for (vector<CandidatePoint>::iterator it1 = candidates2.begin(); it1 != candidates2.end(); ++it1)
    {
      for (vector<CandidatePoint>::iterator it2 = candidates2.begin(); it2 != candidates2.end(); ++it2)
      {
        float d = fabsf(it1->position - it2->position);
        if ((d != 0) && (d < fingerRadius * 1.5)) // XXX
        {
          if (it1->distance < it2->distance)
          {
            it2->remove = true;
          }
          else
          {
            it1->remove = true;
          }
        }
      }
    }
    
    /*
     * ACHIEVING FINGER TRACKING:
     * CONTINUITY OF MOTION FOR EACH
     * CANDIDATE MOVING ALONG THE PATH
     */
    
    auto &fingerList = getFingerList(index);
    
    for (vector<CandidatePoint>::iterator it1 = candidates2.begin(); it1 != candidates2.end(); ++it1)
    {
      if (!it1->remove)
      {
        bool skip = false;
        for (list<FingerRef>::iterator it2 = fingerList.begin(); it2 != fingerList.end(); ++it2)
        {
          FingerRef finger = *it2;
          
          float d = fabsf(it1->position - finger->position);
          if (d < fingerRadius * 1.5) // XXX
          {
            finger->paired = true;
            updateFinger(index, finger->id, it1->position, timestamp, it1->distance);
            
            skip = true;
            continue;
          }
        }
        if (skip) continue;
        
        addFinger(index, idProvider.getUniqueId(0), it1->position, timestamp, it1->distance);
      }
    }
    
    /*
     * REMOVING FINGERS WITH "NO PAIRS",
     * UNLESS THEY'RE FRESHLY ADDED
     */
    
    for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
    {
      if (fingerIsUnpaired(*it))
      {
        removeFinger(index, (*it)->id);
      }
    }
    fingerList.remove_if(fingerIsUnpaired);
    
    for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
    {
      FingerRef finger = *it;
      finger->paired = false;
      finger->updateCount++;
    }
    
    /*
     * SOMEHOW, TWO FINGERS WITH THE SAME
     * POSITION CAN BE PRESENT, SO WE NEED
     * TO REMOVE THE NEWEST ONE
     */
    
    for (list<FingerRef>::iterator it1 = fingerList.begin(); it1 != fingerList.end(); ++it1)
    {
      FingerRef finger1 = *it1;
      
      for (list<FingerRef>::iterator it2 = fingerList.begin(); it2 != fingerList.end(); ++it2)
      {
        FingerRef finger2 = *it2;
        
        if ((it1 != it2) && (finger1->position == finger2->position))
        {
          if (finger1->id < finger2->id)
          {
            finger2->remove = true;
          }
          else
          {
            finger1->remove = true;
          }
        }
      }
    }
    
    for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
    {
      if (Finger::shouldBeRemoved(*it))
      {
        removeFinger(index, (*it)->id);
      }
    }
    fingerList.remove_if(Finger::shouldBeRemoved);
  }
}

void FingerTracker::removeTouch(int index, const glm::vec2 &point)
{
  if (index < capacity)
  {
    auto &fingerList = getFingerList(index);
    
    for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
    {
      removeFinger(index, (*it)->id);
    }
    
    fingerList.clear();
  }
}

std::list<FingerRef>& FingerTracker::getFingerList(int index)
{
  return fingerMap[index];
}

void FingerTracker::addFinger(int index, float position, double timestamp)
{
  addFinger(index, idProvider.getUniqueId(0), position, timestamp, 0);
}

void FingerTracker::addFinger(int index, int id, float position, double timestamp, float distance)
{
  Finger *finger = new Finger;
  finger->index = index;
  finger->id = id;
  finger->position = finger->prevPosition = position;
  finger->timestamp = finger->prevTimestamp = timestamp;
  finger->distance = distance;
  
  getFingerList(index).push_back(FingerRef(finger));

  LOGD << "FINGER ADDED: " << id
    << " [INDEX: " << index
    << "][POSITION: " << position
    << "]" << endl;

  delegate.fingerAdded(finger);
}

void FingerTracker::updateFinger(int index, int id, float position, double timestamp, float distance)
{
  auto &fingerList = getFingerList(index);
  
  for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
  {
    FingerRef finger = *it;
    
    if (finger->id == id)
    {
      finger->prevPosition = finger->position;
      finger->prevTimestamp = finger->timestamp;
      
      finger->position = position;
      finger->timestamp = timestamp;
      finger->distance = distance;
      finger->updateCount++;

      LOGD << "FINGER UPDATED: " << id
        << " [INDEX: "  << index
        << "][POSITION: "<< position
        << "][DELTA: " << position - finger->prevPosition
        << "][DT: " << timestamp - finger->prevTimestamp
        << "][VEL: " << (position - finger->prevPosition) / (timestamp - finger->prevTimestamp)
        << "]" << endl;

      delegate.fingerUpdated(&*finger);
      return;
    }
  }
}

void FingerTracker::removeFinger(int index, int id)
{
  LOGD << "FINGER REMOVED: " << id
    << " [INDEX: "
    << index
    << "]" << endl;
  
  auto &fingerList = getFingerList(index);
  
  for (list<FingerRef>::iterator it = fingerList.begin(); it != fingerList.end(); ++it)
  {
    FingerRef finger = *it;
    
    if (finger->id == id)
    {
      delegate.fingerRemoved(&*finger);
      return;
    }
  }
}
