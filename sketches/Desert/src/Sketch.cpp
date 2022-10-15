#include "Sketch.h"
#include "CinderCamera.h"

using namespace std;
using namespace chr;
using namespace gl;

constexpr float BORDER_HM = 480;
constexpr float BORDER_HP = 480;
constexpr float BORDER_VM = 176;
constexpr float BORDER_VP = 300;

constexpr float TERRAIN_H = 150;

const glm::vec3 COLOR_BACK(1.0f, 1.0f, 0.5f);
const glm::vec4 COLOR_SOIL(0.84f, 0.68f, 0.01f, 1.0f);
const glm::vec4 COLOR_SOIL_WIRE(0.0f, 0.0f, 0.0f, 0.25f);

Sketch::Sketch()
:
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
surface(800, 4, 0.01f, 3, 123456789),
patch(&surface)
{}

void Sketch::setup()
{
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
    viewOX = +30 * clock()->getTime();
}

void Sketch::draw()
{
    glDepthMask(GL_TRUE);
    glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---

    CinderCamera camera;
    camera.setEyePoint(glm::vec3(-1.1223f, -185.783f, 48.6359f));
    camera.setOrientation(glm::quat(0.7915398f, 0.6111088f, -0.003264602f, -5.330436E-4f));
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

    patch.setup(viewOX - BORDER_HM, viewOY - BORDER_VM, BORDER_HM + BORDER_HP, BORDER_VM + BORDER_VP, NoisePatch::MODE_FILL | NoisePatch::MODE_GRID_H);
    patch.update(TERRAIN_H, glm::vec2(0));

    glDepthMask(GL_TRUE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 1.0f);

    patch.drawFill(COLOR_SOIL);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_FALSE);

    patch.drawGrid(COLOR_SOIL_WIRE);
}
