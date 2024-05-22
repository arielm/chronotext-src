#pragma once

#include "chr/Timer.h"

#include "fmod.h"

class AudioManager
{
public:
    void setup();
    void shutdown();
    
    bool isPlaying();
    void togglePlay();
    
    void start(bool mono = true);
    void stop(bool mono = true);
    
    void update();
    void readCallback();
    
    unsigned int getPlayingOffset();
    void setPlayingOffset(unsigned int offset);
    
    double getPlayingRate();
    void setPlayingRate(double rate);
    
    FMOD_CHANNEL* playRun(unsigned int offset, double rate);
    void stopRun(FMOD_CHANNEL *channel);
    void resumeRun(FMOD_CHANNEL *channel);
    void pauseRun(FMOD_CHANNEL *channel);
    void setRunRate(FMOD_CHANNEL *channel, double rate);

protected:
    FMOD_SYSTEM *system = nullptr;
    FMOD_CHANNELGROUP *masterGroup = nullptr;

    FMOD_SOUND *sound = nullptr;
    FMOD_CHANNEL *channel = nullptr;

    FMOD_DSP *dsp = nullptr;
    chr::Timer timer;
    unsigned int playingOffset = 0;
    double playingRate = +1;

    bool playing = false;

    std::string writeError(FMOD_RESULT result);
  FMOD_CHANNEL* playSound(FMOD_SOUND *sound, int loopCount = 0, float volume = 1);
};
