/*
 * Based on SandReader_27.java (Book of Sand sketch)
 *
 * - Press mouse to drop sand
 * - Press and hold 'shift' to remove sand
 * - Press 'x' to remove all sand
 */

#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/TextureShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/Random.h"

#include "common/xf/FontManager.h"

#include "Lines.h"
#include "Letters.h"

class Sketch : public chr::CrossSketch
{
public:
  Sketch();
  virtual ~Sketch() {}

  void setup() final;
  void update() final;
  void draw() final;

  void mouseDragged(int button, float x, float y) final;
  void mousePressed(int button, float x, float y) final;
  void mouseReleased(int button, float x, float y) final;

  void keyDown(int keyCode, int modifiers) final;
  void keyUp(int keyCode, int modifiers) final;

protected:
  struct Shape
  {
    std::vector<std::vector<bool>> grid;
    int width, height;
    int anchorX, anchorY;
    int count = 0;

    Shape(std::vector<std::vector<bool>> grid, int width, int height)
    :
    grid(grid),
    width(width),
    height(height)
    {
      anchorX = width / 2;
      anchorY = height / 2;

      for (int y = 0; y < width; y++)
      {
        for (int x = 0; x < width; x++)
        {
          if (grid[x][y])
          {
            count++;
          }
        }
      }
    }
  };

  chr::gl::shaders::TextureShader textureShader;

  chr::gl::shaders::TextureAlphaShader textureAlphaShader;
  chr::xf::FontManager fontManager;
  std::shared_ptr<chr::xf::Font> font;
  chr::xf::FontSequence sequence;

  static constexpr int W = 598;
  static constexpr int H = 598;
  static constexpr uint32_t BG_COLOR = 0xffffffff;

  uint32_t pixels[W * H];
  int lattice[H + 1][W];

  std::vector<float> heights;
  static constexpr float INTERP_FACTOR = 10;

  static constexpr int AVERAGE = 8;
  static constexpr int N = W / AVERAGE;
  std::vector<float> control_x, control_y;

  CompositePath path;
  Lines lines;
  Letters letters;

  int currentLine = 0;
  bool lineFullyInside = false;

  chr::gl::VertexBatch<chr::gl::XYZ> lineBatch;
  chr::gl::shaders::ColorShader colorShader;

  Shape round20;
  chr::Random random;
  bool rnd_horiz = false;
  bool rnd_cg = false;
  int rnd_col = 0; // used to produce a random color index between 0 and 7...
  int rnd_speed = 0; // used to procuce a random speed between 0 and 3 (that will be translated to an increment between 0.7 and 1)

  bool mouseIsPressed = false;
  float mouseX, mouseY;
  int keyCode;
  bool shiftPressed = false;

  glm::ivec2 convertCoordinates(float x, float y);

  std::vector<std::vector<bool>> createRound(int diameter);
  static void HLine(std::vector<std::vector<bool>> &grid, int x, int y, int w);

  void spread(int x, int y, const Shape &shape, int quantity);
  void clear(int x, int y, const Shape &shape, int quantity);

  void populate();
  void collapse();
  void calculate();
  void computeHeights();
  void computeAverage(int yOffset);
  void grab();

  void setCurrentLine(int index, float position, float velocity);
  void switchLines();

  static chr::gl::Texture createRGBATexture(int w, int h, const void *pixels);
};
