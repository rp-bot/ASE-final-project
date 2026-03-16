#pragma once

#include <atomic>
#include <memory>
#include <juce_audio_basics/juce_audio_basics.h>
#include "SynthSound.h"
#include "SynthVoice.h"
#include "../DSP/WavetableBank.h"
#include "../Threading/AtomicGuiState.h"

namespace Audio
{
    /** Synth engine: owns a JUCE Synthesiser with one SynthVoice (single-note test).
        Renders via 8 oscillators → TrilinearMixer8 → AmpEnvelope.
    */
    class SynthEngine
    {
    public:
        explicit SynthEngine(Threading::AtomicGuiState* guiState) noexcept;
        ~SynthEngine();

        void prepare(double sampleRate, int samplesPerBlock) noexcept;

        /** Render the next audio block from MIDI. Clears and fills buffer. */
        void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) noexcept;

        /** Legacy: render block with no MIDI (silence). */
        void processBlock(juce::AudioBuffer<float>& buffer) noexcept;

        void noteOn(int midiNoteNumber, float velocity) noexcept;
        void noteOff(int midiNoteNumber) noexcept;
        void allNotesOff() noexcept;

        void pitchWheelMoved(int value) noexcept;
        void controllerMoved(int controllerNumber, int value) noexcept;

        int getLastMidiNote() const noexcept;
        float getLastVelocity() const noexcept;
        int getLastPitchWheel() const noexcept;
        int getLastController() const noexcept;
        int getLastControllerValue() const noexcept;

    private:
        double sampleRate_ { 44100.0 };
        int blockSize_ { 0 };

        Threading::AtomicGuiState* m_guiState { nullptr };
        std::unique_ptr<DSP::WavetableBank> m_wavetableBank;
        juce::Synthesiser m_synthesiser;
        bool m_voicesAdded { false };

        std::atomic<int>   lastMidiNote_        { -1 };
        std::atomic<float> lastVelocity_        { 0.0f };
        std::atomic<int>   lastPitchWheel_      { 0 };
        std::atomic<int>   lastController_      { -1 };
        std::atomic<int>   lastControllerValue_ { 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthEngine)
    };
}

