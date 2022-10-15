/*
 * BASED ON https://github.com/cinder/Cinder/blob/master/include/cinder/Camera.h
 */

#pragma once

#include "chr/glm.h"

class CinderCamera
{
public:
    void setPerspective(float verticalFovDegrees, float aspectRatio, float nearPlane, float farPlane);
    void setEyePoint(const glm::vec3 &eyePoint);
    void setOrientation(const glm::quat &orientation);

    void calcMatrices();

    const glm::mat4& getProjectionMatrix() const
    {
        return mProjectionMatrix;
    }

    const glm::mat4& getViewMatrix() const
    {
        return mViewMatrix;
    }

protected:
    glm::vec3 mEyePoint;
    glm::vec3 mViewDirection;
    glm::quat mOrientation;

    float mFov; // vertical field of view in degrees
    float mAspectRatio;
    float mNearClip;
    float mFarClip;

    glm::vec3 mU; // Right vector
    glm::vec3 mV; // Readjust up-vector
    glm::vec3 mW; // Negative view direction

    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;

    float mFrustumLeft, mFrustumRight, mFrustumTop, mFrustumBottom;

    void calcProjection();
    void calcViewMatrix();
};
