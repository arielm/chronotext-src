#include "Tree.h"
#include "UI.h"

class TreeManager
{
public:
  std::shared_ptr<Stream> streamLoaded;
  std::shared_ptr<Tree> treeLoaded;

  std::shared_ptr<Stream> streamRecorded;
  std::shared_ptr<Tree> treeRecorded;

  std::shared_ptr<Tree> createLoadedTree(std::shared_ptr<UI> ui, const chr::InputSource &inputSource);
  std::shared_ptr<Tree> createRecordedTree(std::shared_ptr<UI> ui);

  void mouseMoved(float x, float y);
  void mouseDragged(int button, float x, float y);
  void mousePressed(int button, float x, float y);
  void mouseReleased(int button, float x, float y);

  void keyPressed(uint32_t codepoint);
  void keyDown(int keyCode, int modifiers);

protected:
  void setTreeParams(std::shared_ptr<Tree> tree);
};
