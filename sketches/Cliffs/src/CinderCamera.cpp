#include "CinderCamera.h"

void CinderCamera::setPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane)
{
    mFov = verticalFovDegrees;
    mAspectRatio = aspectRatio;
    mNearClip = nearPlane;
    mFarClip = farPlane;
}

void CinderCamera::setEyePoint(const glm::vec3 &eyePoint)
{
    mEyePoint = eyePoint;
}

void CinderCamera::setOrientation(const glm::quat &orientation)
{
    mOrientation = glm::normalize( orientation );
    mViewDirection = glm::rotate( mOrientation, glm::vec3( 0, 0, -1 ) );
}

void CinderCamera::calcMatrices()
{
    calcViewMatrix();
    calcProjection();
}

void CinderCamera::calcViewMatrix()
{
    mW = -glm::normalize( mViewDirection );
    mU = glm::rotate( mOrientation, glm::vec3( 1, 0, 0 ) );
    mV = glm::rotate( mOrientation, glm::vec3( 0, 1, 0 ) );

    glm::vec3 d( - dot( mEyePoint, mU ), - dot( mEyePoint, mV ), - dot( mEyePoint, mW ) );

    glm::mat4 &m = mViewMatrix;
    m[0][0] = mU.x; m[1][0] = mU.y; m[2][0] = mU.z; m[3][0] =  d.x;
    m[0][1] = mV.x; m[1][1] = mV.y; m[2][1] = mV.z; m[3][1] =  d.y;
    m[0][2] = mW.x; m[1][2] = mW.y; m[2][2] = mW.z; m[3][2] =  d.z;
    m[0][3] = 0.0f; m[1][3] = 0.0f; m[2][3] = 0.0f; m[3][3] = 1.0f;
}

void CinderCamera::calcProjection()
{
    mFrustumTop = mNearClip * tanf((float)M_PI / 180.0f * mFov * 0.5f);
    mFrustumBottom = -mFrustumTop;
    mFrustumRight = mFrustumTop * mAspectRatio;
    mFrustumLeft = -mFrustumRight;

    glm::mat4 &p = mProjectionMatrix;
    p[0][0] = 2.0f * mNearClip / (mFrustumRight - mFrustumLeft);
    p[1][0] = 0.0f;
    p[2][0] = (mFrustumRight + mFrustumLeft) / (mFrustumRight - mFrustumLeft);
    p[3][0] = 0.0f;

    p[0][1] = 0.0f;
    p[1][1] = 2.0f * mNearClip / (mFrustumTop - mFrustumBottom);
    p[2][1] = (mFrustumTop + mFrustumBottom) / (mFrustumTop - mFrustumBottom);
    p[3][1] = 0.0f;

    p[0][2] = 0.0f;
    p[1][2] = 0.0f;
    p[2][2] = -(mFarClip + mNearClip) / (mFarClip - mNearClip);
    p[3][2] = -2.0f * mFarClip * mNearClip / (mFarClip - mNearClip);

    p[0][3] = 0.0f;
    p[1][3] = 0.0f;
    p[2][3] = -1.0f;
    p[3][3] = 0.0f;
}
