#include "Run.h"

using namespace std;

Run::Run(const vector<Brick1>::iterator &startBrick, const vector<Brick1>::iterator &endBrick)
:
startBrick(startBrick),
endBrick(endBrick),
playing(false),
channel(nullptr)
{
    if (endBrick < startBrick)
    {
        assert(false);
    }
}

Run::Run(const vector<Brick1>::iterator &startBrick, const vector<Brick1>::iterator &endBrick, float frequency, double startTime)
:
startBrick(startBrick),
endBrick(endBrick),
playing(false),
channel(nullptr)
{
    if (endBrick < startBrick)
    {
        assert(false);
    }

    setProperties(frequency, startTime);
}

void Run::setProperties(float frequency, double startTime)
{
    if ((frequency == 0) || (startTime < 0))
    {
        throw;
    }

    this->frequency = frequency;
    this->startTime = startTime;

    if (frequency < 0)
    {
        endTime = startTime - (endBrick->endOffset - startBrick->startOffset) / 44100.0 / frequency;
    }
    else
    {
        endTime = startTime + (endBrick->endOffset - startBrick->startOffset) / 44100.0 / frequency;
    }
}

double Run::getDuration()
{
    return endTime - startTime;
}
