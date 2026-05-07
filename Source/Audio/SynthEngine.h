#pragma once

#include <atomic>
#include <juce_audio_basics/juce_audio_basics.h>
#include "VoiceManager.h"
#include "../Threading/AtomicGuiState.h"

namespace juce
{
    class AudioProcessorValueTreeState;
}

namespace Audio
{
    /** Top-level synth engine: owns VoiceManager, parses MIDI in processBlock,
        snapshots parameters once per block, delegates rendering to VoiceManager.
    */
    class SynthEngine
    {
    public:
        explicit SynthEngine (Threading::AtomicGuiState* guiState,
                              juce::AudioProcessorValueTreeState* apvts) noexcept;
        ~SynthEngine() = default;

        void prepare (double sampleRate, int samplesPerBlock) noexcept;

        /** Snapshot parameters, parse MIDI, clear buffer, render via VoiceManager. */
        void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) noexcept;

        /** Legacy: clear buffer only (silence). */
        void processBlock (juce::AudioBuffer<float>& buffer) noexcept;

        void noteOn (int midiChannel, int midiNoteNumber, float velocity) noexcept;
        void noteOff (int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff = true) noexcept;
        void allNotesOff (bool allowTailOff = true) noexcept;

        void pitchWheelMoved (int value) noexcept;
        void controllerMoved (int controllerNumber, int value) noexcept;

        VoiceManager& getVoiceManager() noexcept;
        const VoiceManager& getVoiceManager() const noexcept;

        float getMeterLevelLeft()  const noexcept { return meterLevelLeft_.load  (std::memory_order_relaxed); }
        float getMeterLevelRight() const noexcept { return meterLevelRight_.load (std::memory_order_relaxed); }
        bool isEngineHardOff() const noexcept { return engineHardOff_.load (std::memory_order_relaxed); }
        void resetEngineHardOff() noexcept;

        int getLastMidiNote() const noexcept;
        float getLastVelocity() const noexcept;
        int getLastPitchWheel() const noexcept;
        int getLastController() const noexcept;
        int getLastControllerValue() const noexcept;

    private:
        double m_sampleRate { 0.0 };
        int m_blockSize { 0 };

        Threading::AtomicGuiState* m_guiState { nullptr };
        juce::AudioProcessorValueTreeState* m_apvts { nullptr };

        VoiceManager m_voiceManager;

        std::atomic<int>   lastMidiNote_        { -1 };
        std::atomic<float> lastVelocity_        { 0.0f };
        std::atomic<int>   lastPitchWheel_     { 0 };
        std::atomic<int>   lastController_      { -1 };
        std::atomic<int>   lastControllerValue_ { 0 };

        std::atomic<float> meterLevelLeft_  { -96.0f };
        std::atomic<float> meterLevelRight_ { -96.0f };
        std::atomic<bool> engineHardOff_ { false };
        std::atomic<bool> pendingResetHardOff_ { false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEngine)
    };
}
