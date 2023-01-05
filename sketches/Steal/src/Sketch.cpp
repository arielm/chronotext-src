#include "Sketch.h"

#include "chr/gl/draw/Rect.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr int GRID_SIZE = 45;
static constexpr int CELL_SIZE = 16;
static constexpr float FONT_SIZE = 14;

static constexpr float SPEED = 150;
static constexpr float NOISE_SCALE = 0.001f;

Sketch::Sketch()
:
noise(123456, 3, 0.5f),
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
fogTextureAlphaShader(InputSource::resource("FogTextureAlphaShader.vert"), InputSource::resource("FogTextureAlphaShader.frag"))
{}

void Sketch::setup()
{
    font = fontManager.getFont(InputSource::resource("Menlo_Bold_64.fnt"), XFont::Properties3d());
    font->setShader(fogTextureAlphaShader);
    font->setColor(158 / 255.0f, 158 / 255.0f, 222 / 255.0f, 1);
    font->setSize(FONT_SIZE);

    fillBatch
        .setShader(fogColorShader)
        .setShaderColor(46 / 255.0f, 47 / 255.0f, 158 / 255.0f, 1);

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::draw()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::perspective(60 * D2R, windowInfo.aspectRatio(), 0.1f, 1000.0f);

    Matrix viewMatrix;
    viewMatrix
        .scale(1, -1, 1)
        .translate(0, 0, -500)
        .rotateX(15 * D2R)
        .rotateY(15 * D2R);

    gl::State()
        .setShaderMatrix(viewMatrix * projectionMatrix)
        .setShaderUniform("u_fogDensity", 0.002f)
        .setShaderUniform("u_fogColor", glm::vec3(0, 0, 0))
        .apply();

    // ---

    double now = clock()->getTime();

    fillBatch.clear();
    font->beginSequence(sequence);

    generate(Matrix().translate(-650, -400), now * SPEED, 0);
    generate(Matrix().translate(-650 + GRID_SIZE * CELL_SIZE, -400).rotateY(-HALF_PI), now * SPEED + GRID_SIZE * CELL_SIZE, 0);

    fillBatch.flush();

    font->endSequence();
    font->replaySequence(sequence);
}

void Sketch::generate(const Matrix &matrix, float ox, float oy)
{
    auto characters = font->getCharacters();
    auto count = characters.size();

    float offsetX = font->getOffsetX(u"A", XFont::ALIGN_MIDDLE);
    float offsetY = font->getOffsetY(XFont::ALIGN_MIDDLE);

    for (int ix = 0; ix < GRID_SIZE; ix++)
    {
        for (int iy = 0; iy < GRID_SIZE; iy++)
        {
            float x = ix * CELL_SIZE;
            float y = iy * CELL_SIZE;
            auto value = noise.get((ox + x) * NOISE_SCALE, (oy + y) * NOISE_SCALE);
            auto c = characters[value * count];

            font->addGlyph(matrix, font->getGlyphIndex(c), x + offsetX + CELL_SIZE / 2.0f, y + offsetY + CELL_SIZE / 2.0f);
        }
    }

    draw::Rect()
        .setBounds(0, 0, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE)
        .append(fillBatch, matrix);
}
