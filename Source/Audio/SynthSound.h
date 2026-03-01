#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

namespace Audio
{
    class SynthSound : public juce::SynthesiserSound
    {
    public:
        SynthSound() = default;
        ~SynthSound() override = default;

        bool appliesToNote(int midiNoteNumber) override;
        bool appliesToChannel(int midiChannel) override;
    };
}