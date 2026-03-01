/*
  ==============================================================================

    AtomicGuiStateTest.cpp
    Unit tests for Threading::AtomicGuiState.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Threading/AtomicGuiState.h"

#include <atomic>
#include <chrono>
#include <cmath>
#include <thread>

namespace
{
constexpr float tol = 1e-6f;
}

class AtomicGuiStateTest : public juce::UnitTest
{
public:
    AtomicGuiStateTest() : juce::UnitTest ("AtomicGuiState", "Threading") {}

    void runTest() override
    {
        runDefaultsTest();
        runSetGetTest();
        runThreadedReadWriteTest();
    }

private:
    void runDefaultsTest()
    {
        beginTest ("Defaults: centered cursor and trajectory inactive");

        Threading::AtomicGuiState state;
        const auto p = state.getCursorPosition();

        expectWithinAbsoluteError (p.x, 0.5f, tol);
        expectWithinAbsoluteError (p.y, 0.5f, tol);
        expectWithinAbsoluteError (p.z, 0.5f, tol);
        expect (! state.isTrajectoryActive());
    }

    void runSetGetTest()
    {
        beginTest ("Set/get: scalar + vec3 writers and clamp behavior");

        Threading::AtomicGuiState state;

        state.setCursorPosition (0.25f, 0.75f, 0.5f);
        state.setTrajectoryActive (true);

        auto p = state.getCursorPosition();
        expectWithinAbsoluteError (p.x, 0.25f, tol);
        expectWithinAbsoluteError (p.y, 0.75f, tol);
        expectWithinAbsoluteError (p.z, 0.5f, tol);
        expect (state.isTrajectoryActive());

        state.setCursorPosition (glm::vec3 (2.0f, -1.0f, 0.9f));
        p = state.getCursorPosition();
        expectWithinAbsoluteError (p.x, 1.0f, tol);
        expectWithinAbsoluteError (p.y, 0.0f, tol);
        expectWithinAbsoluteError (p.z, 0.9f, tol);
    }

    void runThreadedReadWriteTest()
    {
        beginTest ("Threaded: concurrent message-thread writes and audio-thread reads");

        Threading::AtomicGuiState state;
        std::atomic<bool> shouldRun { true };
        std::atomic<int> readCount { 0 };
        std::atomic<int> writeCount { 0 };

        std::thread writer ([&]
        {
            int i = 0;
            while (shouldRun.load (std::memory_order_relaxed))
            {
                const float t = static_cast<float> (i % 101) / 100.0f;
                state.setCursorPosition (t, 1.0f - t, t * 0.5f);
                state.setTrajectoryActive ((i % 2) == 0);
                ++i;
                writeCount.store (i, std::memory_order_relaxed);
            }
        });

        bool allValuesFiniteAndInRange = true;
        const auto endTime = std::chrono::steady_clock::now() + std::chrono::milliseconds (200);

        while (std::chrono::steady_clock::now() < endTime)
        {
            const auto p = state.getCursorPosition();
            juce::ignoreUnused (state.isTrajectoryActive());

            const bool valid = std::isfinite (p.x) && std::isfinite (p.y) && std::isfinite (p.z)
                               && p.x >= 0.0f && p.x <= 1.0f
                               && p.y >= 0.0f && p.y <= 1.0f
                               && p.z >= 0.0f && p.z <= 1.0f;

            if (! valid)
            {
                allValuesFiniteAndInRange = false;
                break;
            }

            readCount.fetch_add (1, std::memory_order_relaxed);
        }

        shouldRun.store (false, std::memory_order_relaxed);
        writer.join();

        expect (allValuesFiniteAndInRange, "Read values must stay finite and clamped to [0,1].");
        expect (readCount.load (std::memory_order_relaxed) > 0, "Reader must execute during stress test.");
        expect (writeCount.load (std::memory_order_relaxed) > 0, "Writer must execute during stress test.");
    }
};

static AtomicGuiStateTest atomicGuiStateTest;
