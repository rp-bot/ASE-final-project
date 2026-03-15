#pragma once

#include <atomic>
#include <juce_audio_basics/juce_audio_basics.h>

namespace Audio
{
    /** Minimal synth engine facade.
        For now this just consumes MIDI and produces silence, but it provides
        the API that MidiManager and the processor expect.
    */
    class SynthEngine
    {
    public:
        SynthEngine() = default;
        ~SynthEngine() = default;

        void prepare (double sampleRate, int samplesPerBlock) noexcept;

        /** Render the next audio block. Currently writes silence. */
        void processBlock (juce::AudioBuffer<float>& buffer) noexcept;

        // MIDI event entry points
        void noteOn (int midiNoteNumber, float velocity) noexcept;
        void noteOff (int midiNoteNumber) noexcept;
        void allNotesOff() noexcept;

        void pitchWheelMoved (int value) noexcept;
        void controllerMoved (int controllerNumber, int value) noexcept;

        /** Last MIDI note number received via noteOn, or -1 if none. */
        int getLastMidiNote() const noexcept;
        float getLastVelocity() const noexcept;
        int getLastPitchWheel() const noexcept;
        int getLastController() const noexcept;
        int getLastControllerValue() const noexcept;

    private:
        double sampleRate_ { 44100.0 };
        int blockSize_ { 0 };

        std::atomic<int>   lastMidiNote_        { -1 };
        std::atomic<float> lastVelocity_        { 0.0f };
        std::atomic<int>   lastPitchWheel_      { 0 };
        std::atomic<int>   lastController_      { -1 };
        std::atomic<int>   lastControllerValue_ { 0 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEngine)
    };
}

