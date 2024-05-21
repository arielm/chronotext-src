#include "Sketch.h"

#include <json/value.h>
#include <json/reader.h>

using namespace std;
using namespace chr;
using namespace gl;
using namespace math;

static constexpr float R1 = 9;
static constexpr float R2 = 300;
static constexpr float TURNS = 30;
static constexpr float DIR = -1;

/*
 * LENGTH OF EACH SEGMENT (STROKE AND WALL CURVATURE...)
 */
static constexpr float DD1 = 1.5f; // R1
static constexpr float DD2 = 7.5f; // R2

static constexpr float MIN_H = 1;
static constexpr float MAX_H = 6;

static constexpr float FONT_SIZE = 6.0f;
static constexpr float TERM_GAP = 2;
static constexpr float TERM_PADDING = 1.5f;
static constexpr float TERM_W = 8;

static constexpr float WALL_GRAY = 0.5f;
static constexpr float TEXT_GRAY = 0.25f;
static constexpr float STROKE_ALPHA = 0.5f;

void Sketch::setup()
{
  initButtonLayout();

  // ---

  font = fontManager.getFont(InputSource::resource("Helvetica_Regular_64.fnt"), XFont::Properties3d());

  font->setShader(textureAlphaShader);
  font->setAxis(+1, -1);

  // ---

  capBatch = IndexedVertexBatch<>(GL_TRIANGLES, vertexBuffer);
  wallBatch = IndexedVertexBatch<>(GL_TRIANGLES, vertexBuffer);
  strokeBatch = IndexedVertexBatch<>(GL_LINES, vertexBuffer);

  capBatch
    .setShader(colorShader)
    .setShaderColor(1, 1, 1, 1);

  wallBatch
    .setShader(colorShader)
    .setShaderColor(WALL_GRAY, WALL_GRAY, WALL_GRAY, 1);

  strokeBatch
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, STROKE_ALPHA);

  generate();

  // ---

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
  arcBall = ArcBall(
    windowInfo.size * 0.5f,
    fminf(windowInfo.width, windowInfo.height),
    150,
    glm::quat(0.9212077f, -0.38088596f, -0.06807204f, -0.040847324f));

  buttonLayout.resize(windowInfo.size);
}

void Sketch::update()
{
  buttonLayout.update();
}

void Sketch::draw()
{
  glClearColor(1, 1, 1, 1);

  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ---

  Matrix arcballMatrix(arcBall.getMatrix());
  arcballMatrix.rotateZ(clock()->getTime() * 0.25f);

  auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.width / windowInfo.height, 1.0f, 3000.0f);
  auto mvpMatrix = arcballMatrix * projectionMatrix;

  // ---

  float now = clock()->getTime();

  if (heightFactor < 1)
  {
    heightFactor = constrainf(now / 1.5f, 0, 1);
    generateVertices();
  }

  if (heightFactor == 1)
  {
    float alphaFactor = fminf(1, (now - 1.5f) / 0.25f);
    font->setColor(TEXT_GRAY, TEXT_GRAY, TEXT_GRAY, alphaFactor);
  }

  // ---

  state
    .setShaderMatrix(mvpMatrix)
    .glEnable(GL_DEPTH_TEST)
    .glLineWidth(1)
    .apply();

  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1, 1);

  capBatch.flush();
  wallBatch.flush();

  glDepthMask(GL_FALSE);
  glDisable(GL_POLYGON_OFFSET_FILL);

  strokeBatch.flush();

  if (heightFactor == 1)
  {
    font->replaySequence(sequence);
  }

  // ---

  buttonLayout.draw();
}

void Sketch::parse(const fs::path &resourcePath)
{
  Json::Value root;

  auto stream = chr::getResourceStream(resourcePath);

  if (stream)
  {
    Json::Reader reader;

    if (reader.parse(*stream, root))
    {
      auto trends = root["trends"]; // NAMES SORTED ALPHABETICALLY (FROM OLDER TO NEWER DATES)

      vector<string> keys;
      keys.reserve(trends.size());

      for (const auto &key : trends.getMemberNames())
      {
        keys.push_back(key);
      }

      /*
       * THE MOST RECENT TOPICS MUST BE AT THE CENTER OF THE SPIRAL
       */
      std::reverse(std::begin(keys), std::end(keys));

      map<std::string, shared_ptr<Term>> termMap;

      for (const auto &key : keys)
      {
        const auto itemArray = trends[key];
        int numTerms = itemArray.size();

        for (int j = 0; j < numTerms; ++j)
        {
          auto termText = itemArray[j].get("name", "").asString();
          auto found = termMap.find(termText);

          if (found == termMap.end())
          {
            auto term = make_shared<Term>(termText);
            termMap[termText] = term;

            uniqueTermList.push_back(term);
            termList.push_back(term);
          }
          else
          {
            found->second->frequency++;
            termList.push_back(found->second);
          }
        }
      }
    }
  }
}

float Sketch::getTermHeight(const Term &term) const
{
  return MIN_H + (MAX_H - MIN_H) * (term.frequency - minFreq) / (float) maxFreq;
}

void Sketch::generate()
{
  parse("trends.json");

  minFreq = INT_MAX;
  maxFreq = INT_MIN;

  for (auto term : uniqueTermList)
  {
    int freq = term->frequency;

    if (freq < minFreq) minFreq = freq;
    if (freq > maxFreq) maxFreq = freq;
  }

  // ---

  path = SpiralPath(R1, R2, TURNS, DIR);

  generateText();
  generateBlocks();

  // ---

  size_t numBlocks = blocks.size();
  size_t numSegments = 0;

  for (const auto &block : blocks)
  {
    numSegments += block.size;
  }

  size_t numVertices = (numSegments + numBlocks) * 4;
  vertexBuffer.extendCapacity(numVertices);

  size_t numCapIndices = (numSegments + numBlocks) * 6;
  capBatch.extendIndexCapacity(numCapIndices);

  size_t numWallIndices = (numSegments + numBlocks) * 6 * 2 + numBlocks * 6 * 2;
  wallBatch.extendIndexCapacity(numWallIndices);

  size_t numStrokeIndices = (numSegments * 2) * 4 + (numBlocks * 2 * 2) * 4;
  strokeBatch.extendIndexCapacity(numStrokeIndices);

  generateCapIndices();
  generateWallIndices();
  generateStrokeIndices();
}

void Sketch::generateText()
{
  Matrix matrix;

  font->setSize(FONT_SIZE);
  font->beginSequence(sequence);

  float offsetX = 0;
  float offsetY = -font->getOffsetY(XFont::ALIGN_MIDDLE);

  for (auto term : termList)
  {
    auto text = utils::to<u16string>(term->text);

    float h = getTermHeight(*term);
    offsetX += TERM_PADDING;

    for (auto c : text)
    {
      int glyphIndex = font->getGlyphIndex(c);

      if (glyphIndex >= 0)
      {
        auto value = path.offsetToValue(offsetX);

        matrix
          .setTranslate(value.x, value.y, h)
          .rotateZ(value.az);

        font->addGlyph(matrix, glyphIndex, 0, offsetY);
      }

      offsetX += font->getGlyphAdvance(glyphIndex);
    }

    offsetX += TERM_PADDING + TERM_GAP;
  }

  font->endSequence();
}

void Sketch::generateBlocks()
{
  font->setSize(FONT_SIZE);

  float l = TWO_PI * TURNS;
  float dr = (R2 - R1) / l;
  float DD = (DD2 - DD1) / l;
  float D = 0;

  for (auto term : termList)
  {
    auto text = utils::to<u16string>(term->text);
    float chunkLength = TERM_PADDING * 2 + font->getStringAdvance(text);

    float r = sqrtf(R1 * R1 + 2 * dr * D);
    float d = (r - R1) / dr;
    float segmentLength = DD1 + d * DD;

    size_t size = size_t(chunkLength / segmentLength);
    float h = getTermHeight(*term);

    float d1 = -TERM_W * 0.5f;
    float d2 = +TERM_W * 0.5f;

    Block block(size, h);

    for (int j = 0; j <= size; j++)
    {
      float offset = D + j * (chunkLength / size);

      auto value = path.offsetToValue(offset, d1, d2);
      block.quad[j] = value.quad;
    }

    blocks.push_back(block);

    D += chunkLength + TERM_GAP;
  }
}

void Sketch::generateVertices()
{
  vertexBuffer.clear();

  for (const auto &block : blocks)
  {
    float h = block.h * heightFactor;
    size_t size = block.size;

    for (auto i = 0; i <= size; i++)
    {
      vertexBuffer.add(block.quad[i].x1, block.quad[i].y1, 0);
    }

    for (auto i = 0; i <= size; i++)
    {
      vertexBuffer.add(block.quad[i].x1, block.quad[i].y1, h);
    }

    for (auto i = 0; i <= size; i++)
    {
      vertexBuffer.add(block.quad[i].x2, block.quad[i].y2, 0);
    }

    for (auto i = 0; i <= size; i++)
    {
      vertexBuffer.add(block.quad[i].x2, block.quad[i].y2, h);
    }
  }
}

void Sketch::generateCapIndices()
{
  auto &indices = capBatch.indices();
  size_t position = 0;

  for (const auto &block : blocks)
  {
    size_t size = block.size;

    for (int i = position; i < position + size; i++)
    {
      int i0 = i + (size + 1) * 1; // x1[i], y1[i], h
      int i1 = i + (size + 1) * 3; // x2[i], y2[i], h
      int i2 = i0 + 1; // x1[i + 1], y1[i + 1], h
      int i3 = i1 + 1; // x2[i + 1], y2[i + 1], h

      indices.push_back(i0);
      indices.push_back(i1);
      indices.push_back(i3);
      indices.push_back(i3);
      indices.push_back(i2);
      indices.push_back(i0);
    }

    position += (size + 1) * 4;
  }
}

void Sketch::generateWallIndices()
{
  auto &indices = wallBatch.indices();
  size_t position = 0;

  for (const auto &block : blocks)
  {
    size_t size = block.size;

    int i0 = position + (size + 1) * 0; // x1[0], y1[0], 0
    int i1 = position + (size + 1) * 1; // x1[0], y1[0], h
    //
    int i2 = position + (size + 1) * 2; // x2[0], y2[0], 0
    int i3 = position + (size + 1) * 3; // x2[0], y2[0], h

    indices.push_back(i0);
    indices.push_back(i2);
    indices.push_back(i3);
    indices.push_back(i3);
    indices.push_back(i1);
    indices.push_back(i0);

    int j0 = i0 + size; // x1[size], y1[size], 0
    int j1 = i1 + size; // x1[size], y1[size], h
    //
    int j2 = i2 + size; // x2[size], y2[size], 0
    int j3 = i3 + size; // x2[size], y2[size], h

    indices.push_back(j0);
    indices.push_back(j1);
    indices.push_back(j3);
    indices.push_back(j3);
    indices.push_back(j2);
    indices.push_back(j0);

    for (int i = position; i < position + size; i++)
    {
      int k0 = i + (size + 1) * 0; // x1[i], y1[i], 0
      int k1 = i + (size + 1) * 1; // x1[i], y1[i], h
      int k2 = k0 + 1; // x1[i], y1[i], 0
      int k3 = k1 + 1; // x1[i], y1[i], h

      indices.push_back(k0);
      indices.push_back(k1);
      indices.push_back(k3);
      indices.push_back(k3);
      indices.push_back(k2);
      indices.push_back(k0);

      int l0 = i + (size + 1) * 2; // x2[i], y2[i], 0
      int l1 = i + (size + 1) * 3; // x2[i], y2[i], h
      int l2 = l0 + 1; // x2[i], y2[i], 0
      int l3 = l1 + 1; // x2[i], y2[i], h

      indices.push_back(l0);
      indices.push_back(l2);
      indices.push_back(l3);
      indices.push_back(l3);
      indices.push_back(l1);
      indices.push_back(l0);
    }

    position += (size + 1) * 4;
  }
}

void Sketch::generateStrokeIndices()
{
  auto &indices = strokeBatch.indices();
  size_t position = 0;

  for (const auto &block : blocks)
  {
    int size = block.size;

    int i0 = position + (size + 1) * 0; // x1[0], y1[0], 0
    int i1 = position + (size + 1) * 1; // x1[0], y1[0], h
    //
    int i2 = position + (size + 1) * 2; // x2[0], y2[0], 0
    int i3 = position + (size + 1) * 3; // x2[0], y2[0], h

    indices.push_back(i0);
    indices.push_back(i1);
    //
    indices.push_back(i2);
    indices.push_back(i3);
    //
    //
    indices.push_back(i0);
    indices.push_back(i2);
    //
    indices.push_back(i1);
    indices.push_back(i3);

    int j0 = i0 + size; // x1[size], y1[size], 0
    int j1 = i1 + size; // x1[size], y1[size], h
    //
    int j2 = i2 + size; // x2[size], y2[size], 0
    int j3 = i3 + size; // x2[size], y2[size], h

    indices.push_back(j0);
    indices.push_back(j1);
    //
    indices.push_back(j2);
    indices.push_back(j3);
    //
    //
    indices.push_back(j0);
    indices.push_back(j2);
    //
    indices.push_back(j1);
    indices.push_back(j3);

    for (int j = 0; j < 4; j++)
    {
      for (int i = position; i < position + size; i++)
      {
        indices.push_back(i);
        indices.push_back(i + 1);
      }

      position += size + 1;
    }
  }
}

void Sketch::mouseMoved(float x, float y)
{
  buttonLayout.mouseMoved(x, y);
}

void Sketch::mouseDragged(int button, float x, float y)
{
  if (uiLock->check(&buttonLayout))
  {
    buttonLayout.mouseDragged(button, x, y);
  }
  else if (uiLock->check(this))
  {
    arcBall.mouseDragged(button, x, y);
  }
}

void Sketch::mousePressed(int button, float x, float y)
{
  buttonLayout.mousePressed(button, x, y);

  if (uiLock->acquire(this))
  {
    arcBall.mousePressed(button, x, y);
  }
}

void Sketch::mouseReleased(int button, float x, float y)
{
  buttonLayout.mouseReleased(button, x, y);
  uiLock->release(this);
}

void Sketch::buttonEvent(int id, Button::Action action)
{
  if (id == 0)
  {
    if (fullScreen)
    {
      exitFullScreen();
    }
    else
    {
      enterFullScreen();
    }
  }
}

void Sketch::enterFullScreen()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    emscripten_request_fullscreen("#canvas", 1);
  #endif
}

void Sketch::exitFullScreen()
{
  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    emscripten_exit_fullscreen();
  #endif
}

void Sketch::fullScreenEvent(bool isFullScreen)
{
  fullScreen = isFullScreen;

  Button *button = buttonLayout.getButton(0);

  if (button)
  {
    if (isFullScreen)
    {
      button->setType(Button::TYPE_CLICKABLE);
      button->setTextureBounds(buttonLayout.atlas.getTileBounds(1, 0));
    }
    else
    {
      button->setType(Button::TYPE_PRESSABLE);
      button->setTextureBounds(buttonLayout.atlas.getTileBounds(0, 0));
    }
  }
}

#if defined(CHR_PLATFORM_EMSCRIPTEN)
  EM_BOOL Sketch::fullScreenChangeCallback(int eventType, const EmscriptenFullscreenChangeEvent *e, void *userData)
  {
    reinterpret_cast<Sketch*>(userData)->fullScreenEvent(e->isFullscreen);
    return 0;
  }
#endif

void Sketch::initButtonLayout()
{
  uiLock = make_shared<UILock>();

  TiledAtlas atlas("atlas1.png", 64, 4);

  buttonLayout.setup(uiLock, atlas, 12);
  buttonLayout.setHandler(this);

  #if defined(CHR_PLATFORM_EMSCRIPTEN)
    fullScreenEnabled = true;
    emscripten_set_fullscreenchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, 1, fullScreenChangeCallback);
  #endif

  //

  if (fullScreenEnabled)
  {
    Button::Style style1;
    style1.backgroundColors[Button::STATE_NORMAL] = { 1, 0, 0, 1 };
    style1.colors[Button::STATE_NORMAL] = { 1, 1, 1, 1 };
    style1.backgroundColors[Button::STATE_PRESSED] = { 0, 0, 0, 1 };
    style1.colors[Button::STATE_PRESSED] = { 1, 1, 1, 1 };

    Button buttonR1;
    buttonR1
      .setId(0)
      .setStyle(style1)
      .setTextureBounds(atlas.getTileBounds(0, 0))
      .setType(Button::TYPE_PRESSABLE);
    buttonLayout.addButton(ButtonLayout::RM, buttonR1);
  }
}
