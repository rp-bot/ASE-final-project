#pragma once

#include <atomic>
#include <glm/glm.hpp>
#include "Parameters/ParameterCorners.h"

namespace Threading
{
    /**
     * Lock-free state for 3D cursor position and trajectory mode.
     * Written by the message thread (Editor); read by the audio thread.
     * All coordinates are in [0, 1] for the unit cube.
     */
    class AtomicGuiState
    {
    public:
        AtomicGuiState();
        ~AtomicGuiState() = default;

        void syncFromAPVTS(juce::AudioProcessorValueTreeState& apvts) noexcept;
        CornerParams getCorner(int index) const noexcept;
        void setCorner(int index, const CornerParams& params) noexcept;


        // ---- Writers (message thread only) ----
        void setCursorPosition (float x, float y, float z);
        void setCursorPosition (glm::vec3 position);
        void setTrajectoryActive (bool active);

        // ---- Readers (audio thread safe) ----
        glm::vec3 getCursorPosition() const;
        bool isTrajectoryActive() const;

    private:
        std::atomic<float> m_cursorX;
        std::atomic<float> m_cursorY;
        std::atomic<float> m_cursorZ;
        std::atomic<bool>  m_trajectoryActive;

        struct AtomicCorner
        {
            std::atomic<int> waveform { 0 };
            std::atomic<float> level { 0.0f };
            std::atomic<float> detune { 0.0f };
            std::atomic<float> coarse { 0.0f };
            std::atomic<float> fine { 0.0f };
            std::atomic<float> pan { 0.0f };
        };

        std::array<AtomicCorner, 8> m_corners;

    };
}
