#pragma once

#include <atomic>
#include <glm/glm.hpp>

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
    };
}
