#include "Sketch.h"
#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;

static constexpr uint32_t colors[] = { 0xff2a74b5, 0xff2f80c7, 0xff3c8bd1, 0xff4e96d5, 0xff60a1da, 0xff73acde, 0xff85b7e2, 0xff98c2e7 };

static constexpr int heights_RTL[] = { 556, 555, 555, 554, 554, 553, 553, 552, 553, 552, 551, 550, 549, 549, 548, 548, 547, 546, 545, 544, 544, 543, 543, 543, 542, 541, 540, 540, 539, 539, 538, 538, 537, 537, 537, 536, 535, 534, 533, 532, 531, 530, 529, 529, 529, 528, 528, 527, 527, 526, 525, 525, 524, 523, 523, 522, 522, 521, 520, 520, 519, 518, 518, 517, 516, 516, 515, 514, 514, 513, 512, 511, 511, 510, 510, 509, 509, 508, 508, 507, 507, 506, 505, 504, 504, 503, 502, 502, 501, 500, 499, 499, 499, 498, 498, 497, 497, 496, 495, 495, 494, 493, 492, 491, 491, 490, 490, 489, 489, 488, 488, 487, 486, 485, 485, 485, 484, 483, 482, 482, 482, 481, 480, 479, 478, 477, 477, 476, 475, 475, 474, 474, 474, 473, 472, 471, 470, 469, 468, 467, 467, 466, 466, 465, 465, 464, 464, 464, 463, 462, 461, 460, 460, 459, 458, 458, 457, 456, 456, 456, 456, 456, 456, 456, 456, 455, 454, 454, 453, 453, 453, 453, 452, 452, 451, 451, 451, 451, 450, 450, 450, 449, 449, 450, 449, 450, 450, 451, 450, 450, 450, 451, 450, 451, 451, 451, 451, 451, 452, 452, 452, 453, 453, 453, 452, 453, 453, 454, 454, 455, 454, 454, 455, 455, 455, 455, 456, 455, 455, 455, 455, 455, 455, 455, 456, 456, 457, 457, 457, 458, 459, 458, 459, 459, 459, 460, 460, 460, 461, 461, 461, 461, 461, 461, 462, 462, 463, 463, 464, 464, 464, 465, 466, 466, 467, 467, 468, 469, 469, 469, 470, 470, 471, 472, 472, 473, 473, 474, 474, 474, 475, 476, 477, 477, 478, 479, 480, 481, 481, 481, 481, 482, 481, 482, 483, 484, 483, 484, 485, 486, 486, 486, 486, 486, 487, 486, 486, 487, 486, 486, 486, 486, 486, 486, 485, 485, 485, 485, 484, 484, 483, 483, 482, 483, 483, 482, 481, 480, 479, 480, 479, 478, 478, 478, 477, 477, 476, 476, 476, 475, 474, 473, 472, 471, 471, 470, 471, 470, 469, 469, 468, 467, 466, 466, 465, 465, 464, 464, 463, 462, 462, 462, 461, 460, 460, 459, 458, 457, 457, 457, 456, 456, 455, 456, 457, 456, 455, 454, 453, 453, 453, 453, 452, 451, 451, 450, 449, 449, 449, 448, 448, 448, 447, 448, 447, 446, 445, 446, 445, 445, 445, 444, 443, 443, 443, 442, 441, 440, 440, 439, 439, 439, 438, 437, 437, 437, 437, 436, 436, 435, 434, 434, 433, 433, 433, 432, 432, 431, 430, 429, 429, 428, 427, 426, 425, 425, 425, 424, 424, 423, 422, 422, 421, 420, 420, 419, 419, 418, 417, 416, 415, 415, 414, 413, 412, 412, 411, 410, 410, 409, 408, 407, 408, 407, 407, 406, 405, 404, 403, 402, 402, 401, 400, 400, 400, 399, 398, 398, 397, 397, 396, 395, 394, 394, 393, 392, 392, 392, 391, 391, 390, 390, 389, 388, 388, 387, 386, 385, 385, 384, 384, 383, 383, 382, 382, 381, 380, 380, 379, 379, 378, 378, 377, 377, 376, 375, 374, 374, 374, 373, 372, 371, 370, 370, 370, 369, 368, 367, 366, 366, 365, 365, 365, 364, 363, 362, 361, 361, 360, 360, 360, 359, 359, 358, 357, 356, 355, 354, 354, 353, 353, 352, 351, 351, 350, 349, 349, 349, 348, 347, 346, 345, 345, 344, 343, 343, 342, 341, 341, 340, 340, 340, 339, 339, 338, 337, 336, 335, 335, 334, 333, 332, 332, 332, 331, 330, 330, 330, 329, 329, 328, 328, 327, 327, 326, 325, 324, 325, 324, 324, 323, 323, 322, 321, 320, 319, 319, 318 };
static constexpr int socs_RTL[] = { 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1 };

Sketch::Sketch()
:
letters(path),
round20(createRound(20), 20, 20),
random(12345678)
{}

void Sketch::setup()
{
  font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
  font->setSize(16);
  font->setShader(textureAlphaShader);

  lines.setText(font, utils::readText<u16string>(InputSource::resource("BookOfSand.txt")));
  lines.wrap(W);
  setCurrentLine(0, 9999, 0);

  // ---

  lineBatch
    .setPrimitive(GL_LINES)
    .setShader(colorShader)
    .setShaderColor(0, 0, 0, 1);

  // ---

  for (int x = 0; x < W; x++)
  {
    for (int y = 0; y < H; y++)
    {
      pixels[y * W + x] = 0xffffffff; // AABBGGRR
    }
  }

  for (int x = 0; x < W; x++)
  {
    for (int y = 0; y < H + 1; y++)
    {
      lattice[y][x] = 0;
    }
  }

  // ---

  heights.resize(W);

  for (int x = 0; x < W; x++)
  {
    heights[x] = heights_RTL[x];
    lattice[H][x] = 1 | socs_RTL[x] << 11; // (invisible) bottom border
  }

  populate();

  // ---

  control_x.resize(N + 4);
  control_y.resize(N + 4);

  for (int i = 2; i < N + 2; i++)
  {
    control_x[i] = (i - 2) * AVERAGE + AVERAGE / 2;
  }

  control_x[0] = control_x[1] = 0;
  control_x[control_x.size() - 1] = control_x[control_x.size() - 2] = W;

  // ---

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::update()
{
  if (mouseIsPressed)
  {
    auto mousePosition = convertCoordinates(mouseX, mouseY);

    if (shiftPressed)
    {
      clear(mousePosition.x, mousePosition.y, round20, 30);
    }
    else
    {
      spread(mousePosition.x, mousePosition.y, round20, 15);
    }
  }

  calculate();

  computeHeights();
  computeAverage(-200); // XXX

  // ---

  SplinePath<glm::vec2> spline;
  spline
    .setType(SplinePath<glm::vec2>::TYPE_BSPLINE)
    .setSamplingTolerance(4);

  for (int i = 0; i < control_x.size(); i++)
  {
    spline.add(control_x[i], control_y[i]);
  }

  path.update(spline);

  // ---

  switchLines();

  letters.update();
}

void Sketch::draw()
{
  glDisable(GL_SCISSOR_TEST);

  glClearColor(0xd0 / 255.0f, 0xd0 / 255.0f, 0xd0 / 255.0f, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  float contentScale = getDisplayInfo().contentScale;
  glScissor(contentScale * (windowInfo.width - 600) * 0.5f, contentScale * (windowInfo.height - 600) * 0.5f, contentScale * 600, contentScale * 600);
  glEnable(GL_SCISSOR_TEST);

  // ---

  glm::mat4 projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

  Matrix modelViewMatrix;
  modelViewMatrix
    .translate(windowInfo.size * 0.5f)
    .scale(+1, -1)
    .translate(-W * 0.5f, -H * 0.5f);

  State()
    .setShaderMatrix<MVP>(modelViewMatrix * projectionMatrix)
    .apply();

  // ---

  auto texture = createRGBATexture(W, H, &pixels);

  IndexedVertexBatch<XYZ.UV> batch;
  batch
    .setShader(textureShader)
    .setTexture(texture)
    .setShaderColor(1, 1, 1, 1);

  draw::Rect()
    .setBounds(0, 0, W, H)
    .append(batch);

  batch.flush();

  /*
  lineBatch.clear();

  const auto &points = path.getPoints();
  for (auto i = 0; i < points.size() - 1; i++)
  {
    lineBatch.addVertices(points[i].position, points[i + 1].position);
  }

  lineBatch.flush();
  */

  font->beginSequence(sequence);
  letters.draw();
  font->endSequence();
  font->replaySequence(sequence);
}

void Sketch::mouseDragged(int button, float x, float y)
{
  mouseX = x;
  mouseY = y;
}

void Sketch::mousePressed(int button, float x, float y)
{
  mouseX = x;
  mouseY = y;
  mouseIsPressed = true;
}

void Sketch::mouseReleased(int button, float x, float y)
{
  mouseIsPressed = false;
}

void Sketch::keyDown(int keyCode, int modifiers)
{
  this->keyCode = keyCode;
  shiftPressed = modifiers & KeyEvent::MODIFIER_SHIFT;

  switch (keyCode)
  {
    case KEY_RETURN:
      grab();
      break;

    case KEY_X:
      collapse();
      break;
  }
}

void Sketch::keyUp(int keyCode, int modifiers)
{
  shiftPressed = false;
}

glm::ivec2 Sketch::convertCoordinates(float x, float y)
{
  int xx = math::constrain(int(x - windowInfo.width * 0.5f + W * 0.5f), 0, W - 1);
  int yy = math::constrain(int(y - windowInfo.height * 0.5f + H * 0.5f), 0, H - 1);
  return glm::ivec2(xx, yy);
}

vector<vector<bool>> Sketch::createRound(int diameter)
{
  vector<vector<bool>> grid;
  grid.resize(diameter);

  for (int i = 0; i < diameter; i++)
  {
    grid[i].resize(diameter);
  }

  if (diameter < 1)
  {
    HLine(grid, 0, 0, 1);
  }
  else
  {
    int sx, sy;
    int x, y;
    int d, even;

    x = 0;
    sx = sy = y = diameter / 2;
    d = 1 - diameter / 2;
    even = (diameter % 2 == 0) ? 1 : 0;

    if (even == 0)
    {
      HLine(grid, 0, sy, y + y);
    }

    while (x < y)
    {
      if (d < 0)
      {
        d += x + x + 3;
      }
      else
      {
        d += x + x - y - y + 5;
        y--;
      }
      x++;
      HLine(grid, sx - x, sy + y - even, x + x - even);
      HLine(grid, sx - x, sy - y, x + x - even);
      HLine(grid, sx - y, sy + x - even, y + y - even);
      HLine(grid, sx - y, sy - x, y + y - even);
    }
  }

  return grid;
}

void Sketch::HLine(vector<vector<bool>> &grid, int x, int y, int w)
{
  for (int i = 0; i < w; i++)
  {
    grid[x + i][y] = true;
  }
}

void Sketch::spread(int x, int y, const Shape &shape, int quantity)
{
  float density = fminf(quantity, shape.count) / shape.count;
  float randomThreshold = 1 - density; // asserting that 0 >= density <= 1

  x -= shape.anchorX;
  y -= shape.anchorY;

  for (int iy = 0; iy < shape.height; iy++)
  {
    for (int ix = 0; ix < shape.width; ix++)
    {
      int xx = x + ix;
      int yy = y + iy;
      if (xx >= 0 && yy >= 0 && xx < W && yy < H)
      {
        if (shape.grid[ix][iy] && random.nextFloat() >= randomThreshold)
        {
          lattice[yy][xx] = 2 | (rnd_col << 2) | (rnd_speed << 5);
          pixels[yy * W + xx] = colors[rnd_col];
        }
      }

      rnd_col = (rnd_col + 1) % 8;
      rnd_speed = (rnd_speed + 1) % 4;
    }
  }
}

void Sketch::clear(int x, int y, const Shape &shape, int quantity)
{
  float density = fminf(quantity, shape.count) / shape.count;
  float randomThreshold = 1 - density; // asserting that 0 >= density <= 1

  x -= shape.anchorX;
  y -= shape.anchorY;

  for (int iy = 0; iy < shape.height; iy++)
  {
    for (int ix = 0; ix < shape.width; ix++)
    {
      int xx = x + ix;
      int yy = y + iy;
      if (xx >= 0 && yy >= 0 && xx < W && yy < H)
      {
        if (shape.grid[ix][iy] && (lattice[yy][xx] & 3) == 2 && random.nextFloat() >= randomThreshold)
        {
          lattice[yy][xx] = 0;
          pixels[yy * W + xx] = BG_COLOR;
        }
      }
    }
  }
}

/*
 * Building sand hills based on "starting heights"
 */
void Sketch::populate()
{
  int y0;
  int col, speed;
  for (int x = 0; x < W; x++)
  {
    y0 = (int) heights[x];
    for (int y = y0; y < H - 1; y++)
    {
      col = int(random.nextFloat() * 7 + 0.5);
      speed = int(random.nextFloat() * 3 + 0.5);

      lattice[y + 1][x] = 2 | (col << 2) | (speed << 5);
      pixels[(y + 1) * W + x] = colors[col];
    }
  }
}

void Sketch::collapse()
{
  int offset = 0;
  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      if ((lattice[y][x] & 3) == 2)
      {
        lattice[y][x] = 0;
        pixels[offset + x] = BG_COLOR;
      }
    }
    offset += W;
  }
}

void Sketch::calculate()
{
  int current, cell_v;
  int masked_y, soc;
  bool left, right;
  int side = 0;
  int offset_x = 0, offset_y;
  for (int x = 0; x < W; x++)
  {
    masked_y = H;
    offset_y = W * (H - 1) + (offset_x++);
    for (int y = H - 2; y >= 0; y--)
    {
      offset_y -= W;

      current = lattice[y][x];
      if ((current & 3) == 0)
      {
        masked_y = -1;
      }
      else
      {
        cell_v = ((current >> 7) & 15) + ((current >> 5) & 3) + 7;
        if (cell_v >= 10)
        {
          if ((lattice[y + 1][x] & 3) == 0)
          {
            lattice[y][x] = 0;
            lattice[y + 1][x] = (current & 0xfffff87f) | ((cell_v - 10) << 7);

            pixels[offset_y] = BG_COLOR;
            pixels[offset_y + W] = colors[(current >> 2) & 7];
          }
          else
          {
            // stuck vertically, trying sidewards...
            soc = (masked_y == -1) ? 0 : ((lattice[masked_y][x] >> 11) & 1);

            left = (x > 0) && ((lattice[y][x - 1] & 3) == 0) && ((lattice[y + soc][x - 1] & 3) == 0);
            right = (x < W - 1) && ((lattice[y][x + 1] & 3) == 0) && ((lattice[y + soc][x + 1] & 3) == 0);

            side = 0;
            if (left || right)
            {
              if (left && right)
              {
                side = rnd_horiz ? -1 : 1;
                rnd_horiz ^= true;
              }
              else
              {
                side = left ? -1 : 1;
              }

              lattice[y][x] = 0;
              lattice[y + soc][x + side] = current & 0xfffff87f; // resetting position offset

              pixels[offset_y] = BG_COLOR;
              pixels[offset_y + side + soc * W] = colors[(current >> 2) & 7];

              if (masked_y != -1)
              {
                lattice[masked_y][x] = (lattice[masked_y][x] & 0xfffff7ff) | (rnd_cg ? 0 : 0x800); // 0x800 equals 1 << 11
              }
              rnd_cg ^= true;
            }
            else
            {
              // setting position offset to one step before the maximum
              lattice[y][x] = (current & 0xfffff87f) | 0x480; // 0x480 equals 9 << 7
            }
          }
        }
        else
        {
          lattice[y][x] = (current & 0xfffff87f) | (cell_v << 7);
        }
      }
    }
  }
}

void Sketch::computeHeights()
{
  for (int x = 0; x < W; x++)
  {
    int y = H - 1;
    while ((lattice[y][x] & 3) >= 2 && y > 0)
    {
      y--;
    }
    heights[x] += (y - heights[x]) / INTERP_FACTOR;
  }
}

void Sketch::computeAverage(int yOffset)
{
  int index = 0;
  for (int i = 2; i < N + 2; i++)
  {
    float sum = 0;
    for (int j = 0; j < AVERAGE; j++)
    {
      sum += heights[index++];
    }
    control_y[i] = sum / (float)AVERAGE + yOffset;
  }

  control_y[0] = control_y[1] = heights[0] + yOffset;
  control_y[control_y.size() - 1] = control_y[control_y.size() - 2] = heights[heights.size() - 1] + yOffset;
}

void Sketch::grab()
{
  for (int x = 0; x < W; x++)
  {
    int y = H - 1;
    while ((lattice[y][x] & 3) >= 2 && y > 0)
    {
      y--;
    }
    cout << y << ", ";
  }
  cout << endl;

  for (int x = 0; x < W; x++)
  {
    cout << ((lattice[H][x] >> 11) & 1) << ", ";
  }
  cout << endl;
}

void Sketch::setCurrentLine(int index, float position, float velocity)
{
  letters.setText(font, lines.text.substr(lines.indices[index], lines.lengths[index]), position, velocity);
}

void Sketch::switchLines()
{
  float posFST = letters.position[0];
  float posLST = letters.position[letters.n - 1];

  float LFT = -8; // XXX
  float RGT = letters.path.getLength() + 8; // XXX

  if (posFST >= LFT && posLST <= RGT)
  {
    lineFullyInside = true;
    return;
  }

  if (lineFullyInside)
  {
    if (posFST < LFT && posLST < LFT)
    {
      // a new line is shuffled to the right

      lineFullyInside = false;

      currentLine++;
      if (currentLine > lines.size - 1)
      {
        currentLine = 0;
      }
      setCurrentLine(currentLine, RGT, 0);
    }
    else if (posFST > RGT && posLST > RGT)
    {
      // a new line is shuffled to the left...

      lineFullyInside = false;

      currentLine--;
      if (currentLine < 0)
      {
        currentLine = lines.size - 1;
      }
      setCurrentLine(currentLine, LFT - lines.getLineWidth(currentLine), 0);
    }
  }
  else
  {
    if (posLST > RGT + lines.getLineWidth(currentLine))
    {
      // a new line has been shuffled to the right, but it can't enter screen...
      setCurrentLine(currentLine, RGT, 0);
    }
    else if (posFST < LFT - lines.getLineWidth(currentLine))
    {
      // a new line has been shuffled to the left, but it can't enter screen...
      setCurrentLine(currentLine, LFT - lines.getLineWidth(currentLine), 0);
    }
  }
}

chr::gl::Texture Sketch::createRGBATexture(int w, int h, const void *pixels)
{
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    return Texture(textureId, w, h, GL_RGBA);
}
