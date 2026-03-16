#include "VoiceManager.h"

namespace Audio
{
    VoiceManager::VoiceManager(Threading::AtomicGuiState* guiState) noexcept
        : m_guiState(guiState)
    {
        m_synthesiser.addSound(new SynthSound());
        m_synthesiser.setNoteStealingEnabled(true);
    }

    void VoiceManager::ensureVoicesAndSoundAdded()
    {
        if (m_voicesAdded)
            return;

        m_wavetableBank = std::make_unique<DSP::WavetableBank>();
        constexpr int tableSize = 2048;
        while (m_wavetableBank->getNumWavetables() < 8)
            m_wavetableBank->addWavetable(juce::AudioBuffer<float>(1, tableSize));
        static constexpr DSP::WaveformType types[8] = {
            DSP::WaveformType::Sine, DSP::WaveformType::Sawtooth, DSP::WaveformType::Square, DSP::WaveformType::Triangle,
            DSP::WaveformType::Sine, DSP::WaveformType::Sawtooth, DSP::WaveformType::Square, DSP::WaveformType::Triangle
        };
        for (uint32_t i = 0; i < 8; ++i)
            m_wavetableBank->generateDefaultWavetable(i, types[i], tableSize);

        for (int i = 0; i < MAX_VOICES; ++i)
            m_synthesiser.addVoice(new SynthVoice(m_wavetableBank.get(), m_guiState));

        m_voicesAdded = true;
    }

    void VoiceManager::prepare(double sampleRate, int blockSize)
    {
        ensureVoicesAndSoundAdded();
        m_synthesiser.setCurrentPlaybackSampleRate(sampleRate);
        for (int i = 0; i < MAX_VOICES; ++i)
        {
            if (auto* v = m_synthesiser.getVoice(i))
                static_cast<SynthVoice*>(v)->prepare(sampleRate, blockSize);
        }
    }

    void VoiceManager::noteOn(int midiChannel, int midiNoteNumber, float velocity)
    {
        m_synthesiser.noteOn(midiChannel, midiNoteNumber, velocity);
    }

    void VoiceManager::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff)
    {
        m_synthesiser.noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);
    }

    void VoiceManager::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
    {
        juce::MidiBuffer emptyMidi;
        m_synthesiser.renderNextBlock(outputBuffer, emptyMidi, startSample, numSamples);
    }

    void VoiceManager::allNotesOff(bool allowTailOff)
    {
        for (int ch = 1; ch <= 16; ++ch)
            m_synthesiser.allNotesOff(ch, allowTailOff);
    }

    void VoiceManager::reset()
    {
        for (int ch = 1; ch <= 16; ++ch)
            m_synthesiser.allNotesOff(ch, false);
    }

    juce::Synthesiser& VoiceManager::getSynthesiser() noexcept
    {
        return m_synthesiser;
    }

    const juce::Synthesiser& VoiceManager::getSynthesiser() const noexcept
    {
        return m_synthesiser;
    }
}
