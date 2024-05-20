#include "Stream.h"
#include "Tree.h"
#include "Branch.h"
#include "Reader.h"
#include "Writer.h"

#include "chr/Log.h"
#include "chr/cross/Keyboard.h"

using namespace std;
using namespace chr;

static const u16string FORMAT_STRING = u"CHRONOTEXT_TTC";

Stream::Stream(int type)
:
type(type)
{}

void Stream::setTree(Tree *tree)
{
  this->tree = tree;
}

void Stream::start(int mode, int64_t creationTime)
{
  if (started)
  {
    return;
  }

  if (mode == MODE_LIVE)
  {
    if (recorded || (type != TYPE_RECORDABLE))
    {
      LOGE << "STREAM HAS BEEN ALREADY RECORDED OR IS READ-ONLY!" << endl;
      return;
    }
  }
  else
  {
    if (!recorded && (type == TYPE_RECORDABLE))
    {
      LOGE << "STREAM MUST BE RECORDED FIRST!" << endl;
      return;
    }
    else if ((type == TYPE_READONLY) && !loaded)
    {
      LOGE << "STREAM IS EMPTY!" << endl;
      return;
    }
  }

  this->mode = mode;
  this->creationTime = creationTime;

  clock->setTime(0);
  clock->start();
  started = true;

  if (mode == MODE_PLAYBACK)
  {
    ext_pos = 0;
    previousPlaybackTime = 0;
  }
}

void Stream::stop()
{
  if (started)
  {
    if (mode == MODE_LIVE)
    {
      InFlush();

      duration = getDuration();
      recorded = true;
    }

    started = false;
    clock->stop();
  }
}

int Stream::getDuration()
{
  if ((mode == MODE_LIVE) && !recorded)
  {
    return int(clock->getTime() * 1000);
  }
  else
  {
    return duration;
  }
}

int64_t Stream::getCreationDate()
{
  return creationTime;
}

void Stream::run()
{
  if (!started)
  {
    return;
  }

  clock->update();
  currentMediaTime = int(clock->getTime() * 1000);

  if (mode == MODE_LIVE)
  {
    InFlush();
  }
  else
  {
    setPlaybackMediaTime(currentMediaTime);
  }
}

int Stream::getCurrentMediaTime()
{
  return currentMediaTime;
}

void Stream::setPlaybackMediaTime(int targetTime)
{
  if (!started)
  {
    return;
  }

  if (targetTime >= getDuration())
  {
    tree->endOfMedia();
    return;
  }

  if (targetTime < previousPlaybackTime)
  {
    LOGE << "CAN'T PLAY BACKWARDS!" << endl;
    LOGE << "ext_pos: " << ext_pos << endl;
    LOGE << "targetTime: " << targetTime << endl;
    LOGE << "previousPlaybackTime: " << previousPlaybackTime << endl;

    tree->endOfMedia();
    return;
  }

  int t;
  while (ext_pos < ext_size && (t = ext_track_time[ext_pos]) <= targetTime)
  {
    if (ext_track_event[ext_pos] == EVENT_TEXT)
    {
      textEvent(ext_track_text[ext_pos], t);
    }
    else
    {
      navEvent(ext_track_nav[ext_pos], t);
    }
    ext_pos++;
  }

  previousPlaybackTime = targetTime;
}

void Stream::keyCharIn(char16_t keyChar)
{
  if (started && (mode == MODE_LIVE))
  {
    textIn(keyChar, int(clock->getTime() * 1000));
  }
}

void Stream::keyCodeIn(int keyCode)
{
  if (started && (mode == MODE_LIVE))
  {
    if ((keyCode == KEY_LEFT) || (keyCode == KEY_RIGHT))
    {
      tree->navigator.codeIn(keyCode, int(clock->getTime() * 1000));
    }
    else if ((keyCode == 8) || (keyCode == 13))
    {
      keyCharIn(keyCode);
    }
  }
}

void Stream::mouseIn()
{
  if (started && (mode == MODE_LIVE))
  {
    tree->navigator.mouseIn(int(clock->getTime() * 1000));
  }
}

void Stream::textIn(char16_t c, int t)
{
  if (((c >= ' ') && (c <= 255)) || (c == 8) || (c == 13))
  {
    in_track_time.push_back(t);
    in_track_event.push_back(EVENT_TEXT);
    in_track_text.push_back(c);
    in_track_nav.push_back(0); // dummy
    in_size++;

    // recording could also take place within InFlush()
    if (type == TYPE_RECORDABLE)
    {
      ext_track_time.push_back(t);
      ext_track_event.push_back(EVENT_TEXT);
      ext_track_text.push_back(c);
      ext_track_nav.push_back(0); // dummy
      ext_size++;
    }
  }
}

void Stream::navIn(int id, int t)
{
  in_track_time.push_back(t);
  in_track_event.push_back(EVENT_NAV);
  in_track_nav.push_back(id);
  in_track_text.push_back(0); // dummy
  in_size++;

  // recording could also take place within InFlush()
  if (type == TYPE_RECORDABLE)
  {
    ext_track_time.push_back(t);
    ext_track_event.push_back(EVENT_NAV);
    ext_track_nav.push_back(id);
    ext_track_text.push_back(0); // dummy
    ext_size++;
  }
}

void Stream::InFlush()
{
  // the advantage of recording from here would to record only the successfully-processed events

  if (in_size > 0)
  {
    for (int i = 0; i < in_size; i++)
    {
      int t = in_track_time[i];

      if (in_track_event[i] == EVENT_TEXT)
      {
        textEvent(in_track_text[i], t);
      }
      else
      {
        navEvent(in_track_nav[i], t);
      }
    }

    in_track_time.clear();
    in_track_event.clear();
    in_track_text.clear();
    in_track_nav.clear();
    in_size = 0;
  }
}

bool Stream::textEvent(char16_t c, int t)
{
  auto current = tree->currentBranch;

  if (c == 8)
  {
    return current->remove();
  }
  else if ((c == 10) || (c == 13))
  {
    auto branch = current->addBranch();

    if (branch)
    {
      branch->start(t);

      LOGD << "NEW BRANCH: " << branch->id << endl;
      return tree->switchBranch(branch);
    }
    else
    {
      LOGD << "CAN'T CREATE BRANCH!" << endl;
      return false;
    }
  }
  else
  {
    return current->add(c, t - current->beginning);
  }
}

bool Stream::navEvent(int id, int t)
{
  return tree->switchBranch(tree->branches[id]);
}

/*
 * FORMAT (JAVA TYPES):
 *
 * HEADER:
 * chars:	format-string
 * long:	record creation date (in millis)
 * int:	duration (in millis)
 * int:	number of events (track length)

 * TRACK:
 * for each event:
 *   int:	time
 *   byte:	event-type
 *   char:	text-data
 *   short:	nav-data
 */

bool Stream::load(const InputSource &inputSource)
{
  if ((type != TYPE_READONLY) || loaded)
  {
    LOGE << "STREAM EITHER RECORDABLE OR ALREADY LOADED!" << endl;
    return false;
  }

  Reader reader(inputSource);

  if (reader.readJavaString(14) != FORMAT_STRING)
  {
    LOGE << "BAD STREAM FORMAT!" << endl;
    return false;
  }

  creationTime = reader.readJavaLong();
  duration = reader.readJavaInt();
  ext_size = reader.readJavaInt();

  for (int i = 0; i < ext_size; i++)
  {
    ext_track_time.push_back(reader.readJavaInt());
    ext_track_event.push_back(reader.readJavaByte());
    ext_track_text.push_back(reader.readJavaChar());
    ext_track_nav.push_back(reader.readJavaShort());
  }

  loaded = true;

  return true;
}

bool Stream::save(OutputTarget &outputTarget)
{
  if ((type != TYPE_RECORDABLE) || !recorded)
  {
    LOGE << "STREAM EITHER NOT-RECORDABLE OR EMPTY!" << endl;
    return false;
  }

  Writer writer(outputTarget);

  writer.writeJavaString(FORMAT_STRING);
  writer.writeJavaLong(getCreationDate());
  writer.writeJavaInt(getDuration());
  writer.writeJavaInt(ext_size);

  for (int i = 0; i < ext_size; i++)
  {
    writer.writeJavaInt(ext_track_time[i]);
    writer.WriteJavaByte(ext_track_event[i]);
    writer.writeJavaChar(ext_track_text[i]);
    writer.writeJavaShort(ext_track_nav[i]);
  }

  return true;
}
