#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace Visualization
{
    namespace
    {
        constexpr float kMinRadius = 1.0f;
        constexpr float kMaxRadius = 20.0f;
        constexpr float kMinElevation = 0.f;
        constexpr float kMaxElevation = 1.2f;
        constexpr float kOrbitSensitivity = 0.005f;
        constexpr float kZoomSensitivity = 0.5f;
    }

    Camera::Camera()
    {
        updateCartesianFromSpherical();
    }

    glm::mat4 Camera::getViewMatrix() const
    {
        return glm::lookAt(position_, target_, up_);
    }

    glm::mat4 Camera::getProjectionMatrix(float width, float height) const
    {
        const float aspect = (height > 0.0f) ? (width / height) : 1.0f;
        const float nearPlane = 0.1f;
        const float farPlane = 100.0f;

        return glm::perspective(glm::radians(fovDegrees_), aspect, nearPlane, farPlane);
    }

    glm::vec3 Camera::getForward() const
    {
        return glm::normalize(target_ - position_);
    }

    void Camera::orbit(float deltaX, float deltaY)
    {
        azimuth_ += deltaX * kOrbitSensitivity;
        elevation_ = std::clamp(elevation_ + deltaY * kOrbitSensitivity, kMinElevation, kMaxElevation);

        updateCartesianFromSpherical();
    }

    void Camera::zoom(float delta)
    {
        radius_ = std::clamp(radius_ - delta * kZoomSensitivity, kMinRadius, kMaxRadius);
        updateCartesianFromSpherical();
    }

    void Camera::updateCartesianFromSpherical()
    {
        const float x = radius_ * std::cos(elevation_) * std::cos(azimuth_);
        const float y = radius_ * std::sin(elevation_);
        const float z = radius_ * std::cos(elevation_) * std::sin(azimuth_);

        position_ = glm::vec3(x, y, z) + target_;
    }
}
