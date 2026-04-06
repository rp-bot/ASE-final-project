#pragma once

#include <glm/glm.hpp>

namespace Visualization
{
    class CubeMesh
    {
    public:
        CubeMesh() = default;
        ~CubeMesh() = default;

        // Allocate GL resources (VAO/VBO/EBO) – called once when GL context is ready.
        void initialise();
        void shutdown();

        // Render the cube as a wireframe in the current OpenGL context.
        void render(const glm::mat4& viewMatrix,
                    const glm::mat4& projectionMatrix) const;

        void setBounds(const glm::vec3& minBounds,
                       const glm::vec3& maxBounds);

        const glm::vec3& getMinBounds() const noexcept { return minBounds_; }
        const glm::vec3& getMaxBounds() const noexcept { return maxBounds_; }

    private:
        void updateBuffers();

        glm::vec3 minBounds_{ -1.0f, -1.0f, -1.0f };
        glm::vec3 maxBounds_{  1.0f,  1.0f,  1.0f };

        unsigned int vao_{ 0 };
        unsigned int vbo_{ 0 };
        unsigned int ebo_{ 0 };
        unsigned int shaderProgram_{ 0 };

        bool buffersDirty_{ true };
    };
}

