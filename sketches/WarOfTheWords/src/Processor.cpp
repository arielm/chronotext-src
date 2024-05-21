#include "Processor.h"

#include "chr/utils/Utils.h"

using namespace std;
using namespace pugi;
using namespace chr;
using namespace math;
using namespace xf;
using namespace path;

void Processor::init(Clock::Ref timeBase)
{
  clock = Clock::create(timeBase);
}

void Processor::process(const fs::path &relativePath, FontManager &fontManager)
{
  auto stream = chr::getResourceStream("war.xml");

  if (stream)
  {
    xml_document doc;

    if (doc.load(*stream).status == xml_parse_status::status_ok)
    {
      ArticleParams articleParams;

      articleParams.titleText = parseArticleComponent(doc, "title");
      articleParams.subtitleText = parseArticleComponent(doc, "subtitle");
      articleParams.sourceText = parseArticleComponent(doc, "source");
      articleParams.bodyText = parseArticleComponent(doc, "body");

      article = make_shared<Article>(articleParams, fontManager);
      searchManager.article = article;

      //

      unordered_multimap<int, Term> termMap;

      for (xml_node tag = doc.first_child().child("term"); tag; tag = tag.next_sibling("term"))
      {
        auto term = parseTerm(tag);
        termMap.emplace(term.id, term);
      }

      for (xml_node tag = doc.first_child().child("attack"); tag; tag = tag.next_sibling("attack"))
      {
        int fromId = utils::fromString(tag.attribute("from").value()) - 1;
        int toId = utils::fromString(tag.attribute("to").value()) - 1;

        auto rangeFrom = termMap.equal_range(fromId);
        auto rangeTo = termMap.equal_range(toId);

        for (auto itFrom = rangeFrom.first; itFrom != rangeFrom.second; ++itFrom)
        {
          for (auto itTo = rangeTo.first; itTo != rangeTo.second; ++itTo)
          {
            itFrom->second.addEnemy(itTo->second);
          }
        }
      }

      for (const auto &entry : termMap)
      {
        searchManager.addCandidateTerm(entry.second);
      }

      searchManager.search();
      searchManager.refresh();
    }

    defineCameraPath();
  }
}

u16string Processor::parseArticleComponent(const xml_document &doc, const string &tagName) const
{
  string text = doc.first_child().child("article").child(tagName.data()).text().get();
  trim(text);
  return utils::to<u16string>(text);
}

Term Processor::parseTerm(const pugi::xml_node &node)
{
  int group = utils::fromString(node.attribute("group").value()); // TODO: SHOULD FAIL IF NOT DEFINED

  //

  int hitsPerChar = utils::fromString(node.attribute("hitsPerChar").value());

  if (hitsPerChar)
  {
    hitsPerChar = constrain(hitsPerChar, 1, 10);
  }
  else
  {
    hitsPerChar = 5;
  }

  //

  string text = node.text().get();
  trim(text);

  return Term(group, hitsPerChar, utils::to<u16string>(text));
}

void Processor::trim(std::string &text) const
{
  size_t begin = 0;
  size_t end = text.size();

  while ((begin < end) && shouldBeTrimmed(text[begin]))
  {
    begin++;
  }

  while ((begin < end) && shouldBeTrimmed(text[end - 1]))
  {
    end--;
  }

  size_t len = end - begin;

  if (len != text.size())
  {
    text = text.substr(begin, len);
  }
}

bool Processor::shouldBeTrimmed(char c) const
{
  switch (c)
  {
    case '\r':
    case '\n':
    case ' ':
      return true;

    default:
      return false;
  }
}

void Processor::run()
{
  searchManager.run(clock->getTime());
  cameraManager.run(clock->getTime());
}

void Processor::reset()
{
  clock->restart();

  searchManager.reset();
  cameraManager.reset();
}

void Processor::defineCameraPath()
{
  SplinePath<glm::vec2> spline;

  spline
    .setType(SplinePath<glm::vec2>::TYPE_BSPLINE)
    .setSamplingTolerance(1);

  addToSpline(spline, article, true, -133); // THE PATH BEGINS 133 PIXELS ABOVE THE TOP OF THE FIRST ARTICLE
  addToSpline(spline, article, true, 0); // ADDITIONAL POINT, TO FORCE SPLINE TO PASS-THROUGH

  for (const auto &span : searchManager.getSpans())
  {
    if (span)
    {
      const auto &bounds = span->getBounds();
      spline.add(bounds.x1y1() + bounds.size() * 0.5f);
    }
  }

  addToSpline(spline, article, false, 0);
  addToSpline(spline, article, false, 0); // ADDITIONAL POINT, TO FORCE SPLINE TO PASS-THROUGH

  cameraManager.path
    .setMode(FollowablePath2D::MODE_TANGENT)
    .begin()
    .add(spline.getPolyline())
    .end();
}

void Processor::addToSpline(SplinePath<glm::vec2> &spline, shared_ptr<Article> article, bool top, float verticalOffset)
{
  spline.add(article->x + article->getWidth() / 2, verticalOffset + article->y + (top ? 0 : article->getHeight()));
}
