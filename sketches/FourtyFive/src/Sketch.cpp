#include "Sketch.h"

#include "chr/gl/draw/Circle.h"

using namespace std;
using namespace chr;
using namespace gl;

static constexpr float SIZE = 1000;

static constexpr float R = 600;
static constexpr float SEGMENT_LENGTH = 10;
static constexpr float FONT_SIZE = 28;

void Sketch::setup()
{
    strokeBatch
        .setPrimitive(GL_LINES)
        .setShader(colorShader)
        .setShaderColor(1, 0, 0, 0.2f);

    fillBatch
        .setShader(colorShader)
        .setShaderColor(1, 0, 0, 1);

    font = fontManager.getFont(InputSource::resource("American_Typewriter_Regular_64.fnt"), XFont::Properties2d());
    font->setShader(textureAlphaShader);

    words = utils::readLines<u16string>(InputSource::resource("words.txt"));
    wordCount = words.size();

    // ---

    drawCircles();
    drawDot();

    generate();

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::resize()
{
    scale = windowInfo.height / SIZE;
}

void Sketch::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // ---

    auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, 0.0f, windowInfo.height);

    Matrix viewMatrix;
    viewMatrix
        .translate(windowInfo.center())
        .scale(+scale, -scale);

    gl::State()
        .setShaderMatrix(viewMatrix * projectionMatrix)
        .apply();

    // ---

    fillBatch.flush();
    strokeBatch.flush();

    font->replaySequence(sequence);
}

void Sketch::generate()
{
    int seed = 12892;

    #if defined(CHR_PLATFORM_EMSCRIPTEN)
        double random = EM_ASM_DOUBLE(
            return fxrand();
        );

        seed = random * 100000;
        LOGI << seed << endl;
    #endif

    rnd1 = Random(seed);
    rnd2 = Random(seed);

    font->setSize(FONT_SIZE);
    font->setColor(0, 0, 0, 1);

    font->beginSequence(sequence);

    float r = R;
    float offset = 0;
    float increment = rnd2.nextFloat(-67, +33);

    for (int i = 0; i < 15; i++)
    {
        auto line = generateLine(3);
        drawCircularText(*font, line, glm::vec2(0, 250), r, offset, XFont::ALIGN_MIDDLE);
        r -= font->getHeight() * 1.1f;
        offset += increment;
    }

    font->endSequence();
}

u16string Sketch::generateLine(int numWords)
{
    u16string result;

    for (int i = 0; i < numWords; i++)
    {
        result += words[rnd1.nextInt(wordCount)];
        if (i < numWords - 1) result += u" ";
    }

    return result;
}

void Sketch::drawCircularText(chr::XFont &font, const std::u16string &text, const glm::vec2 &position, float radius, float offset, chr::XFont::Alignment alignY)
{
    float offsetY = font.getOffsetY(alignY);
    Matrix matrix;

    for (auto c : text)
    {
        auto glyphIndex = font.getGlyphIndex(c);
        float halfWidth = font.getGlyphAdvance(glyphIndex) / 2;
        offset += halfWidth;

        if (glyphIndex >= 0)
        {
            float d = offset / radius;
            matrix
                .setTranslate(position.x + sinf(d) * radius, position.y - cosf(d) * radius)
                .rotateZ(d);

            font.addGlyph(matrix, glyphIndex, -halfWidth, offsetY);
        }

        offset += halfWidth;
    }
}

void Sketch::drawDot()
{
    draw::Circle()
        .setSegmentLength(4)
        .setRadius(25)
        .append(fillBatch, Matrix().translate(0, 250));
}

void Sketch::drawCircles()
{
    font->setSize(FONT_SIZE);

    float r = R;
    for (int i = 0; i < 15; i++)
    {
        drawCircle(0, 250, r, SEGMENT_LENGTH);
        r -= font->getHeight() * 1.1f;
    }
}

void Sketch::drawCircle(float x, float y, float r, float segmentLength, float a1, float a2)
{
    vector<glm::vec2> points;

    float aa = fabsf(a2 - a1);
    int n = ceilf(aa * r / segmentLength) + 1;

    for (int i = 0; i < n; i++)
    {
        float dd = a1 + fminf(aa, i * segmentLength / r);
        float xx = +sinf(dd) * r;
        float yy = +cosf(dd) * r;

        points.emplace_back(x + xx, y + yy);
    }

    for (int i = 0; i < n - 1; i++)
    {
        strokeBatch
            .addVertex(points[i])
            .addVertex(points[i + 1]);
    }
}
