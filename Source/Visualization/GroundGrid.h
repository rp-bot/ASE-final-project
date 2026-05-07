#pragma once

#include <glm/glm.hpp>
#include <limits>

namespace Visualization
{
    class GroundGrid
    {
    public:
        GroundGrid() = default;
        ~GroundGrid() = default;

        void initialise();
        void shutdown();

        void render(const glm::mat4& viewMatrix,
                    const glm::mat4& projectionMatrix,
                    float gridY,
                    const glm::vec3& centerXZ) const;

        void setParams(float radius,
                       float spacing,
                       int majorLineEvery);

    private:
        void rebuildGeometry(float gridY, const glm::vec3& centerXZ);

        float radius_{ 10.0f };
        float spacing_{ 0.5f };
        int majorLineEvery_{ 4 };

        // Cached so we rebuild only if plane/center changes.
        mutable float lastGridY_{ std::numeric_limits<float>::quiet_NaN() };
        mutable glm::vec2 lastCenterXZ_{ std::numeric_limits<float>::quiet_NaN(),
                                        std::numeric_limits<float>::quiet_NaN() };

        unsigned int vao_{ 0 };
        unsigned int vbo_{ 0 };
        unsigned int shaderProgram_{ 0 };
        int vertexCount_{ 0 };
    };
}

