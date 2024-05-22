#include "AudioManager.h"

#include "fmod_errors.h"

#include "chr/ResourceBuffer.h"
#include "chr/Log.h"

using namespace std;
using namespace chr;

/*
 * PROBLEM:
 * WITHIN ~6 FRAMES, Channel::getPosition() IS RETURNING TWICE THE SAME VALUE THAN AT THE PREVIOUS FRAME
 * IN CONSEQUENCE, THE ANIMATION IS NOT TOTALLY SMOOTH
 *
 * SOLUTION: USING A DSP-CALLBACK COUPLED WITH ELAPSED-TIME MEASUREMENT
 * AS MENTIONED IN THIS THREAD: http://www.fmod.org/forum/viewtopic.php?f=7&t=14307&p=48987&hilit=getPosition
 */
static FMOD_RESULT F_CALLBACK dspCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
{
    FMOD_DSP *dsp = (FMOD_DSP*)dsp_state->instance;
    void *userData;
    FMOD_DSP_GetUserData(dsp, &userData);
    
    AudioManager *instance = (AudioManager*)userData;
    instance->readCallback();
    
    return FMOD_OK;
}

/*
 * USING "FMOD_INIT_THREAD_UNSAFE" WHEN INTIALIZING FMOD AND "FMOD_ACCURATETIME"
 * WHEN CREATING SOUND MAY (NOT VERIFIED) IMPROVE SYNCHRONIZATION AND ACCURACY
 */
void AudioManager::setup()
{
    FMOD_System_Create(&system, FMOD_VERSION);
    FMOD_RESULT result = FMOD_System_Init(system, 32, FMOD_INIT_THREAD_UNSAFE, nullptr);

    if (result)
    {
        FMOD_System_Release(system);
        system = nullptr;

        throw runtime_error("UNABLE TO INITIALIZE FMOD");
    }
    else
    {
        FMOD_System_GetMasterChannelGroup(system, &masterGroup);
    }
    
    // ---

    auto buffer = getResourceBuffer("chanson - stanzas 1 to 6.wav");

    if (buffer)
    {
        FMOD_CREATESOUNDEXINFO exinfo;
        memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        exinfo.length = static_cast<unsigned int>(buffer->size());

        FMOD_RESULT result = FMOD_System_CreateSound(system, static_cast<const char *>(buffer->data()), FMOD_DEFAULT | FMOD_OPENMEMORY | FMOD_ACCURATETIME, &exinfo, &sound);

        if (result)
        {
            throw runtime_error("FMOD: UNABLE TO CREATE MEMORY-STREAM | REASON: " + writeError(result));
        }
    }
    else
    {
        throw runtime_error("UNABLE TO LOAD SOUND FROM MEMORY");
    }
    
    // ---
    
    FMOD_DSP_DESCRIPTION desc;
    memset(&desc, 0, sizeof(FMOD_DSP_DESCRIPTION));
    desc.read = &dspCallback;
    desc.userdata = this;
    
    FMOD_System_CreateDSP(system, &desc, &dsp);
    FMOD_DSP_SetActive(dsp, true);
    
    FMOD_DSP *dspHead;
    FMOD_ChannelGroup_GetDSP(masterGroup, 0, &dspHead);
    FMOD_DSP_AddInput(dspHead, dsp, nullptr, FMOD_DSPCONNECTION_TYPE_STANDARD);
}

void AudioManager::shutdown()
{
    if (system)
    {
        if (channel)
        {
            FMOD_Channel_Stop(channel);
            channel = nullptr;
        }

        FMOD_DSP_Release(dsp);
        dsp = nullptr;

        FMOD_Sound_Release(sound);
        sound = nullptr;

        FMOD_System_Close(system);
        FMOD_System_Release(system);
        system = nullptr;
    }
}

bool AudioManager::isPlaying()
{
    return playing;
}

void AudioManager::togglePlay()
{
    playing ^= true;
    
    if (channel)
    {
        FMOD_Channel_SetPaused(channel, !playing);
    }
    
    /*
     * XXX: WAS NOT NECESSARY WITH PREVIOUS VERSION OF FMOD-EX (4.44.17)
     */
    FMOD_ChannelGroup_SetMute(masterGroup, !playing);
    update(); // NOT MANDATORY WHEN USING "FMOD_INIT_THREAD_UNSAFE"
}

void AudioManager::start(bool mono)
{
    playing = true;

    if (mono)
    {
        channel = playSound(sound);
        FMOD_DSP_SetBypass(dsp, false);
    }
    
    /*
     * XXX: WAS NOT NECESSARY WITH PREVIOUS VERSION OF FMOD-EX (4.44.17)
     */
    FMOD_ChannelGroup_SetMute(masterGroup, false);
    FMOD_System_Update(system); // NOT MANDATORY WHEN USING "FMOD_INIT_THREAD_UNSAFE"
}

void AudioManager::stop(bool mono)
{
    playing = false;

    if (mono)
    {
        FMOD_DSP_SetBypass(dsp, true);
        
        FMOD_Channel_Stop(channel);
        channel = nullptr;
    }
    
    /*
     * XXX: WAS NOT NECESSARY WITH PREVIOUS VERSION OF FMOD-EX (4.44.17)
     */
    FMOD_ChannelGroup_SetMute(masterGroup, true);
    FMOD_System_Update(system); // NOT MANDATORY WHEN USING "FMOD_INIT_THREAD_UNSAFE"
}

void AudioManager::update()
{
    double delta = timer.getSeconds();

    FMOD_System_Update(system);
    
    if (playing && channel)
    {
        FMOD_Channel_GetPosition(channel, &playingOffset, FMOD_TIMEUNIT_PCM);
        playingOffset = (unsigned int)max<double>(0, playingOffset + playingRate * delta * 44100.0);
    }
}

// ---

void AudioManager::readCallback()
{
    timer.start();
}

unsigned int AudioManager::getPlayingOffset()
{
    return playingOffset;
}

void AudioManager::setPlayingOffset(unsigned int offset)
{
    if (channel)
    {
        playingOffset = offset;
        FMOD_Channel_SetPosition(channel, playingOffset, FMOD_TIMEUNIT_PCM);
    }
}

double AudioManager::getPlayingRate()
{
    return playingRate;
}

void AudioManager::setPlayingRate(double rate)
{
    if (channel)
    {
        playingRate = rate;
        FMOD_Channel_SetFrequency(channel, playingRate * 44100);
    }
}

// ---

FMOD_CHANNEL* AudioManager::playRun(unsigned int offset, double rate)
{
    auto runChannel = playSound(sound);
    
    FMOD_Channel_SetPosition(runChannel, offset, FMOD_TIMEUNIT_PCM);
    FMOD_Channel_SetFrequency(runChannel, rate * 44100);
    
    return runChannel;
}

void AudioManager::stopRun(FMOD_CHANNEL *runChannel)
{
    FMOD_Channel_Stop(runChannel);
}

void AudioManager::pauseRun(FMOD_CHANNEL *runChannel)
{
    FMOD_Channel_SetPaused(runChannel, true);
}

void AudioManager::resumeRun(FMOD_CHANNEL *runChannel)
{
    FMOD_Channel_SetPaused(runChannel, false);
}

void AudioManager::setRunRate(FMOD_CHANNEL *runChannel, double rate)
{
    FMOD_Channel_SetFrequency(runChannel, rate * 44100);
}

string AudioManager::writeError(FMOD_RESULT result)
{
  return string(FMOD_ErrorString(result));
}

FMOD_CHANNEL* AudioManager::playSound(FMOD_SOUND *sound, int loopCount, float volume)
{
    FMOD_CHANNEL *channel;
    FMOD_RESULT result = FMOD_System_PlaySound(system, sound, masterGroup, true, &channel);

    if (result)
    {
        LOGE << "UNABLE TO PLAY SOUND | REASON: " << writeError(result) << endl;
        return nullptr;
    }

    if (loopCount)
    {
        FMOD_Channel_SetLoopCount(channel, loopCount);
        FMOD_Channel_SetMode(channel, FMOD_LOOP_NORMAL);
    }
    else
    {
      FMOD_Channel_SetMode(channel, FMOD_LOOP_OFF);
    }

    FMOD_Channel_SetVolume(channel, volume);
    FMOD_Channel_SetPaused(channel, false);

    return channel;
}
