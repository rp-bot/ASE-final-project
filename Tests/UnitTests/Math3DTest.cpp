/*
  ==============================================================================

    Math3DTest.cpp
    Unit tests for Utils::Math3D (trilinear weights, clamp, distance, lerp).
    All Math3D functions are real-time safe (no allocations, no locks).

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Utils/Math3D.h"

static constexpr float tol = 1e-6f;

class Math3DTest : public juce::UnitTest
{
public:
    Math3DTest() : juce::UnitTest("Math3D", "Utils") {}

    void runTest() override
    {
        runTrilinearWeightsTests();
        runClampTests();
        runDistanceTests();
        runLerpTests();
    }

private:
    void runTrilinearWeightsTests()
    {
        beginTest("Trilinear at corner (0,0,0)");
        {
            auto w = Utils::trilinearWeights(0.f, 0.f, 0.f);
            expectWithinAbsoluteError(w[0], 1.f, tol);
            for (size_t i = 1; i < 8; ++i)
                expectWithinAbsoluteError(w[i], 0.f, tol);
        }

        beginTest("Trilinear at corner (1,1,1)");
        {
            auto w = Utils::trilinearWeights(1.f, 1.f, 1.f);
            for (size_t i = 0; i < 7; ++i)
                expectWithinAbsoluteError(w[i], 0.f, tol);
            expectWithinAbsoluteError(w[7], 1.f, tol);
        }

        beginTest("Trilinear at center (0.5, 0.5, 0.5)");
        {
            auto w = Utils::trilinearWeights(0.5f, 0.5f, 0.5f);
            const float eighth = 1.f / 8.f;
            for (size_t i = 0; i < 8; ++i)
                expectWithinAbsoluteError(w[i], eighth, tol);
        }

        beginTest("Trilinear weights sum to 1.0");
        {
            float pts[][3] = { {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}, {0.5f, 0.5f, 0.5f},
                               {0.2f, 0.7f, 0.3f}, {0.99f, 0.01f, 0.5f} };
            for (const auto& xyz : pts)
            {
                auto w = Utils::trilinearWeights(xyz[0], xyz[1], xyz[2]);
                float sum = 0.f;
                for (float v : w) sum += v;
                expectWithinAbsoluteError(sum, 1.f, tol);
            }
        }

        beginTest("Trilinear clamp: (1.5, 0.5, -0.1) same as (1, 0.5, 0)");
        {
            auto wClamped = Utils::trilinearWeights(1.5f, 0.5f, -0.1f);
            auto wExpected = Utils::trilinearWeights(1.f, 0.5f, 0.f);
            for (size_t i = 0; i < 8; ++i)
                expectWithinAbsoluteError(wClamped[i], wExpected[i], tol);
        }

        beginTest("trilinearWeights(glm::vec3) matches float version");
        {
            glm::vec3 p(0.3f, 0.6f, 0.9f);
            auto wVec = Utils::trilinearWeights(p);
            auto wFloat = Utils::trilinearWeights(0.3f, 0.6f, 0.9f);
            for (size_t i = 0; i < 8; ++i)
                expectWithinAbsoluteError(wVec[i], wFloat[i], tol);
        }
    }

    void runClampTests()
    {
        beginTest("clampToUnitCube vec3: inside unchanged");
        {
            glm::vec3 p(0.5f, 0.5f, 0.5f);
            glm::vec3 q = Utils::clampToUnitCube(p);
            expectWithinAbsoluteError(q.x, 0.5f, tol);
            expectWithinAbsoluteError(q.y, 0.5f, tol);
            expectWithinAbsoluteError(q.z, 0.5f, tol);
        }

        beginTest("clampToUnitCube vec3: (2, -1, 0.5) -> (1, 0, 0.5)");
        {
            glm::vec3 p(2.f, -1.f, 0.5f);
            glm::vec3 q = Utils::clampToUnitCube(p);
            expectWithinAbsoluteError(q.x, 1.f, tol);
            expectWithinAbsoluteError(q.y, 0.f, tol);
            expectWithinAbsoluteError(q.z, 0.5f, tol);
        }

        beginTest("clampToUnitCube float: inside unchanged");
        {
            float x = 0.2f, y = 0.8f, z = 0.5f;
            Utils::clampToUnitCube(x, y, z);
            expectWithinAbsoluteError(x, 0.2f, tol);
            expectWithinAbsoluteError(y, 0.8f, tol);
            expectWithinAbsoluteError(z, 0.5f, tol);
        }

        beginTest("clampToUnitCube float: (2, -1, 0.5) -> (1, 0, 0.5)");
        {
            float x = 2.f, y = -1.f, z = 0.5f;
            Utils::clampToUnitCube(x, y, z);
            expectWithinAbsoluteError(x, 1.f, tol);
            expectWithinAbsoluteError(y, 0.f, tol);
            expectWithinAbsoluteError(z, 0.5f, tol);
        }
    }

    void runDistanceTests()
    {
        beginTest("distanceSquared (0,0,0) vs (1,0,0) = 1");
        {
            float d2 = Utils::distanceSquared(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
            expectWithinAbsoluteError(d2, 1.f, tol);
        }

        beginTest("distance (0,0,0) vs (1,0,0) = 1");
        {
            float d = Utils::distance(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0));
            expectWithinAbsoluteError(d, 1.f, tol);
        }

        beginTest("distanceSquared (0,0,0) vs (1,1,1) = 3");
        {
            float d2 = Utils::distanceSquared(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            expectWithinAbsoluteError(d2, 3.f, tol);
        }

        beginTest("distance (0,0,0) vs (1,1,1) = sqrt(3)");
        {
            float d = Utils::distance(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
            expectWithinAbsoluteError(d, std::sqrt(3.f), tol);
        }
    }

    void runLerpTests()
    {
        beginTest("lerp(a, b, 0) == a");
        {
            glm::vec3 a(1.f, 2.f, 3.f);
            glm::vec3 b(4.f, 5.f, 6.f);
            glm::vec3 r = Utils::lerp(a, b, 0.f);
            expectWithinAbsoluteError(r.x, a.x, tol);
            expectWithinAbsoluteError(r.y, a.y, tol);
            expectWithinAbsoluteError(r.z, a.z, tol);
        }

        beginTest("lerp(a, b, 1) == b");
        {
            glm::vec3 a(1.f, 2.f, 3.f);
            glm::vec3 b(4.f, 5.f, 6.f);
            glm::vec3 r = Utils::lerp(a, b, 1.f);
            expectWithinAbsoluteError(r.x, b.x, tol);
            expectWithinAbsoluteError(r.y, b.y, tol);
            expectWithinAbsoluteError(r.z, b.z, tol);
        }
    }
};

static Math3DTest math3DTest;
