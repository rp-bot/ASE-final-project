#pragma once

#include <glm/glm.hpp>

namespace Visualization
{
    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        void initialise();
        void shutdown();

        void render(const glm::mat4& viewMatrix,
                    const glm::mat4& projectionMatrix);

    private:
        unsigned int vao_{ 0 };
        unsigned int vbo_{ 0 };
        unsigned int shaderProgram_{ 0 };
    };
}

