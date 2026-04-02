#pragma once

#include <array>
#include <glm/glm.hpp>

namespace juce
{
    class AudioProcessorValueTreeState;
}

namespace Threading
{
    class AtomicGuiState;
}

namespace Audio
{
    constexpr int NUM_OSCS = 8;

    /** POD-style per-oscillator parameters (synth / filter / amp groups). */
    struct OscillatorRuntimeParams
    {
        int waveform = 0;
        float level = 0.75f;
        float detune = 0.0f;
        float coarse = 0.0f;
        float fine = 0.0f;
        float pan = 0.0f;

        float filterCutoffHz = 4000.0f;
        float filterResonance = 0.2f;
        float filterKeyTrack = 0.5f;
        float filterDrive = 0.0f;

        float ampAttack = 0.01f;
        float ampDecay = 0.1f;
        float ampSustain = 0.7f;
        float ampRelease = 0.3f;
        float ampLevel = 1.0f;
        float ampVelSens = 0.5f;
    };

    /** Single audio-thread snapshot: cursor/trajectory from AtomicGuiState; osc params from APVTS atomics. */
    struct ParameterSnapshot
    {
        glm::vec3 cursor { 0.5f, 0.5f, 0.5f };
        bool trajectoryActive = false;
        std::array<OscillatorRuntimeParams, NUM_OSCS> osc {};
    };

    /** Lock-free: only atomic loads from APVTS and AtomicGuiState. */
    void fillParameterSnapshot (juce::AudioProcessorValueTreeState& apvts,
                                const Threading::AtomicGuiState* guiState,
                                ParameterSnapshot& out) noexcept;
}
