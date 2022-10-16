#include "Sketch.h"
#include "CinderCamera.h"

using namespace std;
using namespace chr;
using namespace gl;

const glm::vec3 COLOR_BACK(1.0f, 1.0f, 0.5f);
const glm::vec4 COLOR_WATER(0.4f, 0.8f, 1.0f, 1.0f);
const glm::vec4 COLOR_WATER_WIRE(1.0f, 1.0f, 1.0f, 0.333f);

Sketch::Sketch()
:
fogColorShader(InputSource::resource("FogColorShader.vert"), InputSource::resource("FogColorShader.frag")),
patch(&surface)
{}

void Sketch::setup()
{
    patch.setFrontFace(CCW);
    patch.setup(-200, -200, 400, 400, 4, SeaPatch::MODE_FILL | SeaPatch::MODE_GRID_H | SeaPatch::MODE_GRID_V);

    // ---

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::update()
{
    patch.update(clock()->getTime() * 45 / 60.0, 1);
}

void Sketch::draw()
{
    glDepthMask(GL_TRUE);
    glClearColor(COLOR_BACK.r, COLOR_BACK.g, COLOR_BACK.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ---

    CinderCamera camera;
    camera.setEyePoint(glm::vec3(0.416349f, -69.941f, 6.20153f));
    camera.setOrientation(glm::quat(0.73741f, 0.67543936f, 0.0023135322f, 0.0017110059f));
    camera.setPerspective(60, windowInfo.aspectRatio(), 1, 3000);
    camera.calcMatrices();

    Matrix viewMatrix = camera.getViewMatrix();
    viewMatrix.rotateZ(45 * D2R);

    State()
        .setShader(fogColorShader)
        .setShaderMatrix<MVP>(viewMatrix * camera.getProjectionMatrix())
        .setShaderUniform("u_fogDensity", 0.0075f)
        .setShaderUniform("u_fogColor", COLOR_BACK)
        .apply();

    // ---

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0f, 1.0f);

    patch.drawFill(COLOR_WATER);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_FALSE);

    patch.drawGrid(COLOR_WATER_WIRE);
}
