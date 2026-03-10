#include "AtomicGuiState.h"

#include <algorithm>

namespace Threading
{
namespace
{
    constexpr float minCoord = 0.0f;
    constexpr float maxCoord = 1.0f;

    float clampUnit (float value) noexcept
    {
        return std::clamp (value, minCoord, maxCoord);
    }
}

AtomicGuiState::AtomicGuiState()
    : m_cursorX (0.5f),
      m_cursorY (0.5f),
      m_cursorZ (0.5f),
      m_trajectoryActive (false)
{
}

void AtomicGuiState::setCursorPosition (float x, float y, float z)
{
    m_cursorX.store (clampUnit (x), std::memory_order_relaxed);
    m_cursorY.store (clampUnit (y), std::memory_order_relaxed);
    m_cursorZ.store (clampUnit (z), std::memory_order_relaxed);
}

void AtomicGuiState::setCursorPosition (glm::vec3 position)
{
    setCursorPosition (position.x, position.y, position.z);
}

void AtomicGuiState::setTrajectoryActive (bool active)
{
    m_trajectoryActive.store (active, std::memory_order_relaxed);
}

glm::vec3 AtomicGuiState::getCursorPosition() const
{
    return {
        m_cursorX.load (std::memory_order_relaxed),
        m_cursorY.load (std::memory_order_relaxed),
        m_cursorZ.load (std::memory_order_relaxed)
    };
}

bool AtomicGuiState::isTrajectoryActive() const
{
    return m_trajectoryActive.load (std::memory_order_relaxed);
}
}
