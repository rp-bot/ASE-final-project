#include "Math3D.h"
#include <algorithm>
#include <cmath>

namespace Utils
{
    std::array<float, CUBE_CORNERS> trilinearWeights(float x, float y, float z)
    {
        x = std::clamp(x, 0.f, 1.f);
        y = std::clamp(y, 0.f, 1.f);
        z = std::clamp(z, 0.f, 1.f);

        std::array<float, CUBE_CORNERS> w{};
        for (int k = 0; k < 2; ++k)
            for (int j = 0; j < 2; ++j)
                for (int i = 0; i < 2; ++i)
                {
                    int c = 4 * k + 2 * j + i;
                    w[static_cast<size_t>(c)] = (i ? x : (1.f - x)) * (j ? y : (1.f - y)) * (k ? z : (1.f - z));
                }
        return w;
    }

    std::array<float, CUBE_CORNERS> trilinearWeights(glm::vec3 position)
    {
        return trilinearWeights(position.x, position.y, position.z);
    }

    glm::vec3 clampToUnitCube(glm::vec3 position)
    {
        return glm::clamp(position, 0.f, 1.f);
    }

    void clampToUnitCube(float& x, float& y, float& z)
    {
        x = std::clamp(x, 0.f, 1.f);
        y = std::clamp(y, 0.f, 1.f);
        z = std::clamp(z, 0.f, 1.f);
    }

    float distanceSquared(glm::vec3 a, glm::vec3 b)
    {
        glm::vec3 d = a - b;
        return glm::dot(d, d);
    }

    float distance(glm::vec3 a, glm::vec3 b)
    {
        return std::sqrt(distanceSquared(a, b));
    }

    glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t)
    {
        return glm::mix(a, b, t);
    }
}
