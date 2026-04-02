#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>
#include "ParameterSnapshot.h"
#include "SynthVoice.h"
#include "SynthSound.h"
#include "../DSP/WavetableBank.h"

namespace Audio
{
    constexpr int MAX_VOICES = 16;

    class VoiceManager
    {
    public:
        VoiceManager() noexcept;
        ~VoiceManager() = default;

        void prepare (double sampleRate, int blockSize);

        /** Call once per audio block before MIDI handling so noteOn sees current parameters. */
        void setBlockParameterSnapshot (const ParameterSnapshot& snapshot) noexcept;

        void noteOn (int midiChannel, int midiNoteNumber, float velocity);
        void noteOff (int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);

        void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);

        void allNotesOff (bool allowTailOff = true);

        void reset();

        juce::Synthesiser& getSynthesiser() noexcept;
        const juce::Synthesiser& getSynthesiser() const noexcept;

    private:
        void ensureVoicesAndSoundAdded();

        juce::Synthesiser m_synthesiser;
        std::unique_ptr<DSP::WavetableBank> m_wavetableBank;
        ParameterSnapshot m_blockSnapshot {};
        bool m_voicesAdded { false };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoiceManager)
    };
}
