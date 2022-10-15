#include "Sketch.h"
#include "CinderCamera.h"

using namespace std;
using namespace chr;
using namespace gl;

constexpr float BORDER_HM = 100; // horizontal-minus
constexpr float BORDER_HP = 300; // horizonal-plus
constexpr float BORDER_VM = 200; // vertical-minus
constexpr float BORDER_VP = 120; // vertical-plus

constexpr float TERRAIN_H = 150;

const glm::vec3 COLOR_BACK(1.0f, 1.0f, 0.5f);
const glm::vec4 COLOR_WALL(1.0f, 0.6f, 0.2f, 1.0f);
const glm::vec4 COLOR_WALL_WIRE(0.0f, 0.0f, 0.0f, 0.2f);

Sketch::Sketch()
:
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
surface(400, 2, 0.01f, 4, 123456789),
patch(&surface)
{}

void Sketch::setup()
{
    surface.setBaseFunction(bind(&Sketch::cliffNoiseBase, this, std::placeholders::_1, std::placeholders::_2));
    surface.generateBase();

    patch.setFrontFace(CCW);

    // ---

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::update()
{
    viewOX = 300 + 12 * clock()->getTime();
}

void Sketch::draw()
{
    glDepthMask(GL_TRUE);
    glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---

    CinderCamera camera;
    camera.setEyePoint(glm::vec3(-100, -125, 46.3448f));
    camera.setOrientation(glm::quat(0.87340266f, 0.111865565f, -0.4609995f, 0.110152125f));
    camera.setPerspective(60, windowInfo.aspectRatio(), 1, 2000);
    camera.calcMatrices();

    Matrix viewMatrix = camera.getViewMatrix();
    viewMatrix.translate(-viewOX, -viewOY, -(surface.noiseMin + (surface.noiseMax - surface.noiseMin) * 0.5f) * TERRAIN_H);

    State()
        .setShader(fogColorShader)
        .setShaderMatrix<MVP>(viewMatrix * camera.getProjectionMatrix())
        .setShaderUniform("u_fogDensity", 0.005f)
        .setShaderUniform("u_fogColor", COLOR_BACK)
        .apply();

    // ---

    patch.setup(viewOX - BORDER_HM, viewOY - BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H | NoisePatch::MODE_GRID_V);
    patch.update(TERRAIN_H, glm::vec2(0));

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 1.0f);

    patch.drawFill(COLOR_WALL);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_FALSE);

    patch.drawGrid(COLOR_WALL_WIRE);
}

float Sketch::cliffNoiseBase(float x, float y)
{
    return posterize(surface.tilingNoise1d(y, 0.05f), 0.1f) * surface.tilingNoise2d(x, y, surface.noiseScale);
}

float Sketch::posterize(float x, float factor)
{
    return floorf(x / factor) * factor;
}
