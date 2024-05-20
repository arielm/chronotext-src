#pragma once

#include "Stream.h"
#include "Navigator.h"
#include "NavHistory.h"
#include "EditHistory.h"
#include "EventCaster.h"

class Branch;
class UI;

class Tree : public EventCaster
{
public:
  static constexpr int cursor_period = 500; // millis

  enum
  {
    EVENT_ENDOFMEDIA
  };

  enum
  {
    LAYER_TREE,
    LAYER_MASK,
    LAYER_NAVIGATOR
  };

  float cam_fov;
  float cam_elevation, cam_azimuth, cam_twist;
  float cam_x, cam_y, cam_distance;

  float curvature_maxAngle;
  int curvature_timeThreshold;
  float curvature_interpFactor;
  float vanishDistance;
  float slope;

  std::shared_ptr<Branch> rootBranch, currentBranch;
  std::vector<std::shared_ptr<Branch>> branches;
  NavHistory navHistory;
  EditHistory editHistory;
  Navigator navigator;

  std::shared_ptr<Stream> stream;
  bool started = false;

  Tree(std::shared_ptr<Stream> stream, std::shared_ptr<UI> ui);

  void start(int mode, int64_t creationTime);
  void stop();
  void endOfMedia();

  int branchAdded(std::shared_ptr<Branch> branch);
  void branchStopped(std::shared_ptr<Branch> branch);
  void branchEdited(std::shared_ptr<Branch> branch);
  bool switchBranch(std::shared_ptr<Branch> to);
  void run();
  void draw();

protected:
  std::shared_ptr<UI> ui;

  void draw(int layerId);
  void drawTree();
  void drawMask();
};
