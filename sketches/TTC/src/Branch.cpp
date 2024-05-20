#include "Branch.h"
#include "Tree.h"

#include "chr/Log.h"

using namespace std;
using namespace chr;
using namespace gl;

Branch::Branch(Tree *tree, shared_ptr<Branch> parent, int position, shared_ptr<XFont> font, float fontSize)
:
tree(tree),
parent(parent),
position(position),
font(font),
fontSize(fontSize)
{
  childGap = 0.5f * fontSize;
}

void Branch::start(int t)
{
  beginning = t;
  started = true;
  previous.reset();
  size = 0;
}

void Branch::stop()
{
  stream_char.clear();
  stream_time.clear();
  started = false;

  for (int i = childBranches.size() - 1; i >= 0; i--)
  {
    auto child = childBranches[i];

    if (child->started)
    {
      child->stop();
    }
  }

  tree->branchStopped(shared_from_this());

  LOGD << "DELETED BRANCH: " << id << endl;
}

shared_ptr<Branch> Branch::addBranch()
{
  return (size == 0) ? nullptr : addBranchAt(size - 1);
}

shared_ptr<Branch> Branch::addBranchAt(int pos)
{
  auto branch = getChildAt(pos);

  if (branch)
  {
    if (!branch->started)
    {
      tree->branchAdded(branch);
      return branch;
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    branch = make_shared<Branch>(tree, shared_from_this(), pos, font, fontSize);
    childBranches.push_back(branch);
    childCount++;

    childIndexes.resize(pos + 1);
    childIndexes[pos] = childCount;

    branch->id = tree->branchAdded(branch);
    return branch;
  }
}

bool Branch::add(char16_t c, int t)
{
  stream_char.push_back(c);
  stream_time.push_back(t);
  size++;

  tree->branchEdited(shared_from_this());
  return true;
}

bool Branch::remove()
{
  if (size > 0)
  {
    auto child = getChildAt(size - 1);
    if (child && child->started)
    {
      // if the current position is a "node": no character is deleted, instead, a navigation operation occurs...
      if (tree->switchBranch(child))
      {
        return true;
      }
      // navigation couldn't take place (e.g. the target branch is not active anymore): continue & do delete...
    }

    size--;
    stream_char.resize(size); // XXX
    stream_time.resize(size); // XXX

    tree->branchEdited(shared_from_this());
    return true;
  }
  else if (isRoot)
  {
    return false;
  }
  else
  {
    // the beginning of a branch has been reached...
    stop();

    if (!tree->switchBranch(previous))
    {
      tree->switchBranch(parent); // TODO: follow this part to see if it's a real solution or only some patchy workaround...
    }
    return true;
  }
}

shared_ptr<Branch> Branch::getChildAt(int pos)
{
  if (childIndexes.size() > pos)
  {
    int index = childIndexes[pos];
    if (index != 0)
    {
      return childBranches[index - 1];
    }
  }

  return nullptr;
}

void Branch::setPrevious(shared_ptr<Branch> branch)
{
  if (branch->started)
  {
    previous = branch;
  }
}

bool Branch::isEmpty()
{
  return size == 0;
}

void Branch::drawFrom(Matrix &matrix)
{
  drawToStart(matrix, size, 0, false);
}

void Branch::drawToStart(Matrix &matrix, int startingPosition, float startingDistance, bool gap)
{
  if (startingDistance >= tree->vanishDistance)
  {
    return;
  }

  Matrix::Stack stack;
  font->setSize(fontSize);

  curvature.clear();
  float global_curvature = 0;
  float local_curvature;
  int old_time = 0;

  for (int i = 0; i < startingPosition; i++)
  {
    int time = stream_time[i];
    int delta = time - old_time;
    old_time = time;

    if (delta >= tree->curvature_timeThreshold)
    {
      local_curvature = 0;
      global_curvature = 0;
    }
    else
    {
      local_curvature = (1 - (delta / tree->curvature_timeThreshold)) * tree->curvature_maxAngle;
    }

    global_curvature += (local_curvature - global_curvature) * tree->curvature_interpFactor;
    curvature.push_back(global_curvature);
  }

  float ss = sinf(tree->slope);

  if (gap)
  {
    matrix.translate(-childGap, 0, -childGap * ss);
  }

  float d = startingDistance;

  for (int i = startingPosition - 1; i >= 0; i--)
  {
    if (((this == tree->currentBranch.get()) || (((this != tree->currentBranch.get()) && (i != startingPosition - 1)))))
    {
      auto child = getChildAt(i);

      if (child && child->started)
      {
        matrix
          .push(stack)
          .rotateZ(HALF_PI);
        child->drawToEnd(matrix, 0, d, true);
        matrix.pop(stack);

        font->setSize(fontSize);
      }
    }

    float opacity = 1 - (d / tree->vanishDistance);

    if (opacity <= 0)
    {
      break; // no need to draw further...
    }

    font->setColor(1, 1, 1, opacity); // XXX

    char16_t c = stream_char[i];
    int glyphIndex = font->getGlyphIndex(c);
    float w = font->getGlyphAdvance(glyphIndex);
    d += w;

    if (glyphIndex > -1)
    {
      matrix
        .push(stack)
        .rotateY(tree->slope);
      font->addGlyph(matrix, glyphIndex, font->getOffsetX(c, XFont::ALIGN_RIGHT), font->getOffsetY(XFont::ALIGN_MIDDLE));
      matrix.pop(stack);
    }

    matrix
      .translate(-w, 0, w * ss)
      .rotateZ((w / font->getSize()) * curvature[i]);
  }

  if (!isRoot)
  {
    matrix
      .rotateZ(-HALF_PI)
      .push(stack);
    parent->drawToStart(matrix, position + 1, d, true);
    matrix.pop(stack);
    parent->drawToEnd(matrix, position + 1, d, true);
  }
}

void Branch::drawToEnd(Matrix &matrix, int startingPosition, float startingDistance, bool gap)
{
  if (startingDistance >= tree->vanishDistance)
  {
    return;
  }

  Matrix::Stack stack;
  font->setSize(fontSize);

  float global_curvature = 0;
  float local_curvature;
  int old_time = 0;

  for (int i = 0; i < startingPosition; i++)
  {
    float time = stream_time[i];
    float delta = time - old_time;
    old_time = time;

    if (delta >= tree->curvature_timeThreshold)
    {
      local_curvature = 0;
      global_curvature = 0;
    }
    else
    {
      local_curvature = (1 - (delta / tree->curvature_timeThreshold)) * tree->curvature_maxAngle;
    }

    global_curvature += (local_curvature - global_curvature) * tree->curvature_interpFactor;
  }

  float ss = sinf(tree->slope);

  if (gap)
  {
    matrix.translate(childGap, 0, childGap * ss);
  }

  float d = startingDistance;

  for (int i = startingPosition; i < size; i++)
  {
    float time = stream_time[i];
    float delta = time - old_time;
    float old_time = time;

    if (delta >= tree->curvature_timeThreshold)
    {
      local_curvature = 0;
      global_curvature = 0;
    }
    else
    {
      local_curvature = (1 - (delta / tree->curvature_timeThreshold)) * tree->curvature_maxAngle;
    }

    global_curvature += (local_curvature - global_curvature) * tree->curvature_interpFactor;

    // ---

    float opacity = 1 - (d / tree->vanishDistance);

    if (opacity <= 0)
    {
      break; // no need to draw further...
    }

    font->setColor(1, 1, 1, opacity); // XXX

    char16_t c = stream_char[i];
    int glyphIndex = font->getGlyphIndex(c);
    float w = font->getGlyphAdvance(glyphIndex);
    d += w;

    if (glyphIndex > -1)
    {
      matrix
        .push(stack)
        .rotateX(tree->slope);
      font->addGlyph(matrix, glyphIndex, font->getOffsetX(c, XFont::ALIGN_LEFT), font->getOffsetY(XFont::ALIGN_MIDDLE));
      matrix.pop(stack);
    }

    matrix
      .translate(w, 0, w * ss)
      .rotateZ(-(w / font->getSize()) * curvature[i]);

    // ---

    auto child = getChildAt(i);

    if (child && child->started)
    {
      matrix
        .push(stack)
        .rotateZ(HALF_PI);
      child->drawToEnd(matrix, 0, d, true);
      matrix.pop(stack);

      font->setSize(fontSize);
    }
  }
}
