#pragma once

#include <memory>

class FingerData
{
public:
    virtual ~FingerData() {}
};

typedef std::shared_ptr<class Finger> FingerRef;

class Finger
{
public:
    int id;
    float position, prevPosition;
    double timestamp, prevTimestamp;
    
    int updateCount;
    bool paired;
    bool remove;
    
    int index;
    float distance;
    
    FingerData *data;
    
    Finger() : updateCount(0), paired(false), remove(false), data(nullptr) {}
    
    ~Finger()
    {
        if (data)
        {
            delete data;
        }
    }
    
    static bool shouldBeRemoved(const FingerRef finger)
    {
        return finger->remove;
    }
};
