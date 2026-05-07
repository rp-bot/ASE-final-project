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

void AtomicGuiState::syncFromAPVTS(juce::AudioProcessorValueTreeState& apvts) noexcept
{
    for (int i = 0; i < 8; ++i) { //NOTE: ASSUMING WE ARE KEEPING 8; TODO: REPLACE WITH GLOBAL LATER
        const auto waveform = apvts.getRawParameterValue(ParameterIDs::cornerWaveform(i))->load();
        const auto level = apvts.getRawParameterValue(ParameterIDs::cornerLevel(i))->load();
        const auto detune = apvts.getRawParameterValue(ParameterIDs::cornerDetune(i))->load();
        const auto coarse = apvts.getRawParameterValue(ParameterIDs::cornerCoarse(i))->load();
        const auto fine = apvts.getRawParameterValue(ParameterIDs::cornerFine(i))->load();
        const auto pan= apvts.getRawParameterValue(ParameterIDs::cornerPan(i))->load();

        auto& c = m_corners[i];

        c.waveform.store(static_cast<int>(waveform), std::memory_order_relaxed);
        c.level.store(level, std::memory_order_relaxed);
        c.detune.store(detune, std::memory_order_relaxed);
        c.coarse.store(coarse, std::memory_order_relaxed);
        c.fine.store(fine, std::memory_order_relaxed);
        c.pan.store(pan, std::memory_order_relaxed);
    }
}

CornerParams AtomicGuiState::getCorner(int index) const noexcept
{
    const auto& c = m_corners[index];
    return {
        static_cast<DSP::WaveformType>(c.waveform.load(std::memory_order_relaxed)),
        c.level.load(std::memory_order_relaxed),
        c.detune.load(std::memory_order_relaxed),
        c.coarse.load(std::memory_order_relaxed),
        c.fine.load(std::memory_order_relaxed),
        c.pan.load(std::memory_order_relaxed)
    };
}

void AtomicGuiState::setCorner(int index, const CornerParams& p) noexcept
{
    auto& c = m_corners[index];
    c.waveform.store(static_cast<int>(p.waveform), std::memory_order_relaxed);
    c.level.store(p.level, std::memory_order_relaxed);
    c.detune.store(p.detune,std::memory_order_relaxed);
    c.coarse.store(p.coarse,std::memory_order_relaxed);
    c.fine.store(p.fine,std::memory_order_relaxed);
    c.pan.store(p.pan, std::memory_order_relaxed);
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

void AtomicGuiState::setCubeRotation (glm::quat worldFromLocal) noexcept
{
    const glm::quat q = glm::normalize (worldFromLocal);
    m_cubeQw.store (q.w, std::memory_order_relaxed);
    m_cubeQx.store (q.x, std::memory_order_relaxed);
    m_cubeQy.store (q.y, std::memory_order_relaxed);
    m_cubeQz.store (q.z, std::memory_order_relaxed);
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

glm::quat AtomicGuiState::getCubeRotation() const noexcept
{
    return glm::quat (
        m_cubeQw.load (std::memory_order_relaxed),
        m_cubeQx.load (std::memory_order_relaxed),
        m_cubeQy.load (std::memory_order_relaxed),
        m_cubeQz.load (std::memory_order_relaxed));
}

bool AtomicGuiState::isTrajectoryActive() const
{
    return m_trajectoryActive.load (std::memory_order_relaxed);
}
}
