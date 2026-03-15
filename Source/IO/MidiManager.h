#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

namespace Audio
{
    class SynthEngine;
}

namespace IO
{
    /**
     * MidiManager routes host-provided MIDI (from juce::MidiBuffer) into the audio engine.
     *
     * It does not open hardware MIDI devices; instead, VolumetricSynthAudioProcessor
     * calls processMidiBuffer() each block with the DAW's MidiBuffer.
     */
    class MidiManager
    {
    public:
        explicit MidiManager (Audio::SynthEngine& synthEngine) noexcept;

        /** Consume the host's MIDI buffer and forward decoded events into SynthEngine. */
        void processMidiBuffer (const juce::MidiBuffer& midi);

        /** 0 = omni, 1–16 = specific MIDI channel to listen on. */
        void setMidiChannel (int channel) noexcept;

    private:
        Audio::SynthEngine& synthEngine_;
        int midiChannel_ { 0 }; // 0 = omni

        bool channelMatches (int messageChannel) const noexcept;

        void handleNoteOn (int channel, int noteNumber, float velocity);
        void handleNoteOff (int channel, int noteNumber, float velocity);
        void handlePitchWheel (int channel, int value);
        void handleController (int channel, int controller, int value);
    };
} // namespace IO