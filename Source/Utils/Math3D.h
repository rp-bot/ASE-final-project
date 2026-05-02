#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

namespace Utils
{
    // Number of corners in the cube (for TrilinearMixer8)
    constexpr int CUBE_CORNERS = 8;

    /**
     * Corner order for trilinear weights (index = 4*k + 2*j + i for corner (i,j,k) in {0,1}^3):
     * 0: (0,0,0), 1: (1,0,0), 2: (0,1,0), 3: (1,1,0),
     * 4: (0,0,1), 5: (1,0,1), 6: (0,1,1), 7: (1,1,1)
     */

    /**
     * Compute trilinear interpolation weights for a point in the unit cube [0,1]^3.
     * The cube has 8 corners; returns 8 coefficients (one per corner) that sum to 1.0.
     * Used by TrilinearMixer8 to blend 8 oscillators from (x, y, z) position.
     *
     * @param x, y, z  Position in unit cube [0, 1]. Values outside are clamped.
     * @return Array of 8 gains for corners in a consistent order (e.g. 000, 100, 010, 110, 001, 101, 011, 111).
     */
    std::array<float, CUBE_CORNERS> trilinearWeights(float x, float y, float z);

    /**
     * Overload using glm::vec3.
     */
    std::array<float, CUBE_CORNERS> trilinearWeights(glm::vec3 position);

    /**
     * Converts a cursor stored in axis-aligned global/world-normalized [0,1]^3 (offset from cube
     * center mapped as (position+1)/2 per world axis, no cube rotation) into local [0,1]^3 blend
     * coordinates for trilinear weights. worldFromLocal rotates local cube axes into world.
     */
    glm::vec3 globalUnitToLocalBlendUnit(glm::vec3 globalUnit, glm::quat worldFromLocal);

    /**
     * Clamp position to unit cube [0, 1]^3.
     */
    glm::vec3 clampToUnitCube(glm::vec3 position);

    /**
     * Clamp position to unit cube [0, 1]^3 (component-wise).
     */
    void clampToUnitCube(float& x, float& y, float& z);

    /**
     * Squared distance between two points (avoids sqrt for comparisons).
     */
    float distanceSquared(glm::vec3 a, glm::vec3 b);

    /**
     * Distance between two points.
     */
    float distance(glm::vec3 a, glm::vec3 b);

    /**
     * Linear interpolation between two vec3 values.
     */
    glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t);
}
