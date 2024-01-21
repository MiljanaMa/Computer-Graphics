#include "camera.h"

Camera::Camera() {
    SetOrthogonal();
    updateVectors();
}

void
Camera::Move(float dx, float dy, float dt) {
    mPosition += (dx * mRight + dy * mFront) * mMoveSpeed * dt;
    updateVectors();
}
void
Camera::SetOrthogonal() {
    mPosition = glm::vec3(86.0f, 101.5f, 72.0f);
    mPitch = -44.0f;
    mYaw = -140.0f;
    mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mVelocity = glm::vec3(0.0f);
    mMoveSpeed = 30.0f;
    mLookSpeed = 100.0f;
    updateVectors();
}

void
Camera::UpDown(int direction) {
    mPosition.y = mPosition.y + direction * 0.5;
    updateVectors();
}

void
Camera::Rotate(float dx, float dy, float dt) {
    float RotateVelocity = mLookSpeed * dt;
    mYaw += dx * RotateVelocity;
    mPitch += dy * RotateVelocity;

    if (mPitch > 89.0f) {
        mPitch = 89.0f;
    }
    if (mPitch < -89.0f) {
        mPitch = -89.0f;
    }

    updateVectors();
}

glm::vec3
Camera::GetPosition() {
    return mPosition;
}

glm::vec3
Camera::GetTarget() {
    mFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront.y = sin(glm::radians(mPitch));
    mFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront = glm::normalize(mFront);
    return mPosition + mFront;
}

glm::vec3
Camera::GetUp() {
    return mUp;
}

void
Camera::updateVectors() {
    mFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront.y = sin(glm::radians(mPitch));
    mFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
    mFront = glm::normalize(mFront);
    mRight = glm::normalize(glm::cross(mFront, mWorldUp));
    mUp = glm::normalize(glm::cross(mRight, mFront));
}