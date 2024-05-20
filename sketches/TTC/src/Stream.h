#pragma once

#include "chr/time/FrameClock.h"
#include "chr/FileSystem.h"

class Tree;

class Stream
{
public:
  enum
  {
    MODE_LIVE,
    MODE_PLAYBACK
  };

  enum
  {
    TYPE_RECORDABLE,
    TYPE_READONLY
  };

  enum
  {
    EVENT_TEXT,
    EVENT_NAV
  };

  int mode;
  int type;
  chr::FrameClock::Ref clock = chr::FrameClock::create();

  bool recorded = false;
  bool loaded = false;

  Stream(int type);

  void setTree(Tree *tree);
  void start(int mode, int64_t creationTime);
  void stop();

  int getDuration();
  int64_t getCreationDate();
  void run();
  int getCurrentMediaTime();
  void setPlaybackMediaTime(int targetTime);
  void keyCharIn(char16_t keyChar);
  void keyCodeIn(int keyCode);
  void mouseIn();
  void textIn(char16_t c, int t);
  void navIn(int id, int t);

  bool load(const chr::InputSource &inputSource);
  bool save(chr::OutputTarget &outputTarget);

protected:
  // an interleaved stream used as a "physical" (mouse & keyboard) input buffer in LIVE mode
  std::vector<int> in_track_time;
  std::vector<int> in_track_event;
  std::vector<char16_t> in_track_text;
  std::vector<int> in_track_nav;
  int in_size = 0;

  // an interleaved stream used as a data-source both for LIVE (writing to it) & PLAYBACK (reading from it) modes
  std::vector<int> ext_track_time;
  std::vector<int> ext_track_event;
  std::vector<char16_t> ext_track_text;
  std::vector<int> ext_track_nav;
  int ext_size = 0;

  // used for playback
  int ext_pos = 0;
  int currentMediaTime = 0;
  int previousPlaybackTime = 0;

  bool started = false;
  int duration = 0;
  int64_t creationTime = 0;

  Tree *tree = nullptr;

  void InFlush();
  bool textEvent(char16_t c, int t); //	returns false if the event was not processed
  bool navEvent(int id, int t); // returns false if the event was not processed
};
