#pragma once

#include "Brick1.h"

#include "fmod.h"

class Run
{
public:
    std::vector<Brick1>::iterator startBrick;
    std::vector<Brick1>::iterator endBrick;
    
    float frequency;
    double startTime;
    
    double endTime;
    bool playing;
    FMOD_CHANNEL *channel;
    
    Run(const std::vector<Brick1>::iterator &startBrick, const std::vector<Brick1>::iterator &endBrick);
    Run(const std::vector<Brick1>::iterator &startBrick, const std::vector<Brick1>::iterator &endBrick, float frequency, double startTime);

    void setProperties(float frequency, double startTime);
    double getDuration();
};
