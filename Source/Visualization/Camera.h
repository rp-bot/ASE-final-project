#pragma once

#include <glm/glm.hpp>

namespace Visualization
{
    class Camera
    {
    public:
        Camera();

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix(float width, float height) const;
        glm::vec3 getPosition() const noexcept { return position_; }
        float getRadius() const noexcept { return radius_; }
        /** Unit vector from camera toward look target (into the scene). */
        glm::vec3 getForward() const;

        void orbit(float deltaX, float deltaY);
        void zoom(float delta);

    private:
        void updateCartesianFromSpherical();

        glm::vec3 position_{};
        glm::vec3 target_{ 0.0f, 0.0f, 0.0f };
        glm::vec3 up_{ 0.0f, 1.0f, 0.0f };

        float fovDegrees_{ 45.0f };
        float radius_{ 5.0f };
        float azimuth_{ 0.785398f };   // π/4
        float elevation_{ 0.955317f }; // ~asin(1/√3): camera in (1,1,1) direction so all three axes visible
    };
}

