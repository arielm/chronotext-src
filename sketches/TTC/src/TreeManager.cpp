#include "TreeManager.h"

using namespace std;
using namespace chr;

shared_ptr<Tree> TreeManager::createLoadedTree(shared_ptr<UI> ui, const InputSource &inputSource)
{
  streamLoaded = make_shared<Stream>(Stream::TYPE_READONLY);

  if (streamLoaded->load(inputSource))
  {
    treeLoaded = make_shared<Tree>(streamLoaded, ui);
    setTreeParams(treeLoaded);
  }
  else
  {
    treeLoaded.reset();
  }

  return treeLoaded;
}

shared_ptr<Tree> TreeManager::createRecordedTree(shared_ptr<UI> ui)
{
  streamRecorded = make_shared<Stream>(Stream::TYPE_RECORDABLE);

  treeRecorded = make_shared<Tree>(streamRecorded, ui);
  setTreeParams(treeRecorded);

  return treeRecorded;
}

void TreeManager::mouseMoved(float x, float y)
{
  if (treeRecorded)
  {
    treeRecorded->stream->mouseIn();
  }
}

void TreeManager::mouseDragged(int button, float x, float y)
{
  if (treeRecorded)
  {
    treeRecorded->stream->mouseIn();
  }
}

void TreeManager::mousePressed(int button, float x, float y)
{
  if (treeRecorded)
  {
    treeRecorded->stream->mouseIn();
  }
}

void TreeManager::mouseReleased(int button, float x, float y)
{
  if (treeRecorded)
  {
    treeRecorded->stream->mouseIn();
  }
}

void TreeManager::keyPressed(uint32_t codepoint)
{
  if (treeRecorded)
  {
    treeRecorded->stream->keyCharIn((char16_t) codepoint);
  }
}

void TreeManager::keyDown(int keyCode, int modifiers)
{
  if (treeRecorded)
  {
    treeRecorded->stream->keyCodeIn(keyCode);
  }
}

void TreeManager::setTreeParams(shared_ptr<Tree> tree)
{
  tree->curvature_maxAngle = 9 * D2R; // (radians) per font-size (distance)
  tree->curvature_timeThreshold = 1000; // millis, after this period: curvature is null
  tree->curvature_interpFactor = 0.1f; // between 0 & 1, the lower it is the more time it takes for curvature to change
  tree->vanishDistance = 2000; // beyond this distance, no text is shown (strongly related to font-size)
  tree->slope = -5 * D2R;
}
