#pragma once

#include "SearchManager.h"
#include "CameraManager.h"

#include "chr/FileSystem.h"
#include "chr/path/SplinePath.h"
#include "chr/time/Clock.h"

#include "common/xf/FontManager.h"

#include <pugixml.hpp>

class Processor
{
public:
  SearchManager searchManager;
  CameraManager cameraManager;
  std::shared_ptr<Article> article;

  chr::Clock::Ref clock;

  void init(chr::Clock::Ref timeBase);
  void process(const fs::path &relativePath, chr::xf::FontManager &fontManager);
  void run();
  void reset();

protected:
  std::u16string parseArticleComponent(const pugi::xml_document &doc, const std::string &tagName) const;
  Term parseTerm(const pugi::xml_node &node);

  void trim(std::string &text) const;
  bool shouldBeTrimmed(char c) const;

  void defineCameraPath();
  void addToSpline(chr::path::SplinePath<glm::vec2> &spline, std::shared_ptr<Article> article, bool top, float verticalOffset);
};
