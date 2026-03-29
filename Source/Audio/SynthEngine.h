#pragma once

#include <atomic>
#include <juce_audio_basics/juce_audio_basics.h>
#include "VoiceManager.h"
#include "../Threading/AtomicGuiState.h"

namespace Audio
{
    /** Top-level synth engine: owns VoiceManager, parses MIDI in processBlock,
        delegates note on/off and rendering to VoiceManager.
    */
    class SynthEngine
    {
    public:
        explicit SynthEngine(Threading::AtomicGuiState* guiState) noexcept;
        ~SynthEngine() = default;

        void prepare(double sampleRate, int samplesPerBlock) noexcept;

        /** Parse MIDI, update voices, clear buffer, render via VoiceManager. */
        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) noexcept;

        /** Legacy: clear buffer only (silence). */
        void processBlock(juce::AudioBuffer<float>& buffer) noexcept;

        void noteOn(int midiChannel, int midiNoteNumber, float velocity) noexcept;
        void noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff = true) noexcept;
        void allNotesOff(bool allowTailOff = true) noexcept;

        void pitchWheelMoved(int value) noexcept;
        void controllerMoved(int controllerNumber, int value) noexcept;

        VoiceManager& getVoiceManager() noexcept;
        const VoiceManager& getVoiceManager() const noexcept;

        int getLastMidiNote() const noexcept;
        float getLastVelocity() const noexcept;
        int getLastPitchWheel() const noexcept;
        int getLastController() const noexcept;
        int getLastControllerValue() const noexcept;

    private:
        double m_sampleRate { 44100.0 };
        int m_blockSize { 0 };

        VoiceManager m_voiceManager;

        std::atomic<int>   lastMidiNote_        { -1 };
        std::atomic<float> lastVelocity_        { 0.0f };
        std::atomic<int>   lastPitchWheel_     { 0 };
        std::atomic<int>   lastController_      { -1 };
        std::atomic<int>   lastControllerValue_ { 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthEngine)
    };
}
