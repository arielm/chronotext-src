#pragma once

#include <AudioUnit/AudioUnit.h>

class AudioLoopDelegate
{
public:
    virtual void renderCallback(int bufferSize, float *bufferData, double timeStamp) = 0;
};

class AudioLoop
{
    AudioComponentInstance audioUnit;
    bool initialized;
    
    static OSStatus staticRenderCallback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
    {
        AudioLoop *instance = (AudioLoop*)inRefCon;
        if (instance->delegate)
        {
            float *bufferData = (float*)ioData->mBuffers[0].mData;
            double timeStamp = inTimeStamp->mSampleTime / 44100.0;
            
            instance->delegate->renderCallback(inNumberFrames, bufferData, timeStamp);
        }
        
        return noErr;
    }

public:
    AudioLoopDelegate *delegate;

    AudioLoop() : delegate(nullptr), initialized(false) {}

    bool init();
    void shutdown();
    
    void start();
    void stop();
};
