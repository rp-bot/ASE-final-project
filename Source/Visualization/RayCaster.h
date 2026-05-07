#pragma once

#include <glm/glm.hpp>

namespace Visualization
{
    class Camera;

    struct Ray
    {
        glm::vec3 origin{};
        glm::vec3 direction{}; // normalised
    };

    class RayCaster
    {
    public:
        explicit RayCaster(const Camera* camera = nullptr) noexcept;

        void setCamera(const Camera* camera) noexcept;

        // Convert viewport-relative screen coordinates (0..1 in X/Y) into a world-space ray.
        Ray castRay(float screenX,
                    float screenY,
                    float viewportWidth,
                    float viewportHeight) const;

        // Ray–axis-aligned cube intersection helper.
        // Returns true if the ray intersects the cube volume; hitPoint is always clamped
        // to [minBounds, maxBounds] even when returning false.
        bool intersectCube(const Ray& ray,
                           const glm::vec3& minBounds,
                           const glm::vec3& maxBounds,
                           glm::vec3& hitPoint) const noexcept;

    private:
        const Camera* camera_{ nullptr };
    };
}

