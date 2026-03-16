#include "SynthVoice.h"
#include "SynthSound.h"
#include "../Utils/ScopedDenormals.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

namespace Audio
{
    SynthVoice::SynthVoice(DSP::WavetableBank* wavetableBank, Threading::AtomicGuiState* guiState)
        : m_mixer(guiState)
    {
        for (size_t i = 0; i < 8; ++i)
        {
            m_oscillators[i] = std::make_unique<DSP::WavetableOscillator>(wavetableBank);
            static_cast<DSP::WavetableOscillator*>(m_oscillators[i].get())->setWavetable(static_cast<int>(i));
        }
    }

    SynthVoice::~SynthVoice() = default;

    void SynthVoice::prepare(double sampleRate, int blockSize)
    {
        m_sampleRate = sampleRate;

        m_oscillatorOutputs.setSize(8, blockSize);
        m_mixerOutput.setSize(1, blockSize);

        for (auto& osc : m_oscillators)
            osc->prepare(sampleRate);
        m_mixer.prepare(sampleRate);
        m_envelope.prepare(sampleRate);

        m_envelope.setADSR(0.01f, 0.1f, 0.7f, 0.3f);
    }

    bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
    {
        return dynamic_cast<SynthSound*>(sound) != nullptr;
    }

    void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* /*sound*/,
                              int currentPitchWheelPosition)
    {
        m_midiNote = midiNoteNumber;
        m_currentVelocity = velocity;
        m_currentPitchWheelPosition = currentPitchWheelPosition;
        m_isActive = true;

        updateOscillatorFrequency();
        for (auto& osc : m_oscillators)
            osc->reset();
        m_envelope.noteOn();
    }

    void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
    {
        m_envelope.noteOff();
        if (!allowTailOff)
        {
            m_envelope.reset();
            clearCurrentNote();
            m_isActive = false;
        }
    }

    void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
    {
        m_currentPitchWheelPosition = newPitchWheelValue;
        updateOscillatorFrequency();
    }

    void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
    {
    }

    void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
    {
        Utils::ScopedDenormals scopedDenormals;

        const int maxSamples = m_oscillatorOutputs.getNumSamples();
        if (numSamples <= 0 || maxSamples <= 0)
            return;
        numSamples = std::min(numSamples, maxSamples);
        if (m_mixerOutput.getNumSamples() < numSamples)
            return;

        for (int s = 0; s < numSamples; ++s)
        {
            for (size_t i = 0; i < 8; ++i)
            {
                if (m_oscillators[i] != nullptr)
                    m_oscillatorOutputs.setSample(static_cast<int>(i), s, m_oscillators[i]->processSample());
            }
        }

        m_mixer.processBlock(m_oscillatorOutputs, m_mixerOutput, 0, numSamples);

        const int numChannels = std::min(outputBuffer.getNumChannels(), 2);
        constexpr float kHeadroomGain = 0.02f;  // ~-34 dB; audible, headroom for 8 oscillators
        for (int s = 0; s < numSamples; ++s)
        {
            const float envVal = m_envelope.processSample();
            const float mixed = m_mixerOutput.getSample(0, s);
            const float gain = kHeadroomGain * mixed * envVal * m_currentVelocity;

            const int outIndex = startSample + s;
            outputBuffer.addSample(0, outIndex, gain);
            if (numChannels >= 2)
                outputBuffer.addSample(1, outIndex, gain);

            if (!m_envelope.isActive())
            {
                clearCurrentNote();
                m_isActive = false;
                break;
            }
        }
    }

    void SynthVoice::updateOscillatorFrequency()
    {
        const double baseFreq = juce::MidiMessage::getMidiNoteInHertz(m_midiNote);
        const float bendSemitones = 2.0f * (m_currentPitchWheelPosition - 8192) / 8192.0f;
        const float freq = static_cast<float>(baseFreq * std::pow(2.0, static_cast<double>(bendSemitones) / 12.0));

        for (auto& osc : m_oscillators)
        {
            if (osc != nullptr)
                osc->setFrequency(freq);
        }
    }
}
