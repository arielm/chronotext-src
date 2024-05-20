#include "common/xf/Font.h"

#include "chr/gl/Matrix.h"

class Tree;

class Branch : public std::enable_shared_from_this<Branch>
{
public:
  std::vector<char16_t> stream_char;
  std::vector<int> stream_time;
  int size;

  bool started;
  int beginning; // relative to tree's beginning

  int id; // index in tree's global branch list
  bool isRoot = false;

  Branch() = default;
  Branch(Tree *tree, std::shared_ptr<Branch> parent, int position, std::shared_ptr<chr::XFont> font, float fontSize);

  void start(int t);
  void stop();

  std::shared_ptr<Branch> addBranch();
  std::shared_ptr<Branch> addBranchAt(int pos);
  bool add(char16_t c, int t);
  bool remove();
  std::shared_ptr<Branch> getChildAt(int pos);
  void setPrevious(std::shared_ptr<Branch> branch);
  bool isEmpty();

  void drawFrom(chr::gl::Matrix &matrix);
  void drawToStart(chr::gl::Matrix &matrix, int startingPosition, float startingDistance, bool gap);
  void drawToEnd(chr::gl::Matrix &matrix, int startingPosition, float startingDistance, bool gap);

protected:
  std::shared_ptr<chr::XFont> font;
  float fontSize;

  std::vector<float> curvature; // used for temporary calculations

  std::vector<std::shared_ptr<Branch>> childBranches;
  std::vector<int> childIndexes; // holds either 0 or the index (-1) of a branch in childBranches
  int childCount = 0;

  std::shared_ptr<Branch> parent, previous;
  Tree *tree = nullptr;

  int position; // relative to parent's text

  float childGap;
};
