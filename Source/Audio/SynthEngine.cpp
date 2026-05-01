#include "SynthEngine.h"

#include "ParameterSnapshot.h"
#include <juce_audio_processors/juce_audio_processors.h>

#include "../Parameters/ParameterIDs.h"

namespace Audio
{
    namespace
    {
        constexpr float kHardOffClipThresholdDb = 10.0f;
        const float kHardOffClipThresholdLinear = juce::Decibels::decibelsToGain(kHardOffClipThresholdDb);
    }

    SynthEngine::SynthEngine(Threading::AtomicGuiState *guiState,
                             juce::AudioProcessorValueTreeState *apvts) noexcept
        : m_guiState(guiState),
          m_apvts(apvts)
    {
    }

    void SynthEngine::prepare(double sampleRate, int samplesPerBlock) noexcept
    {
        m_sampleRate = sampleRate;
        m_blockSize = samplesPerBlock;
        m_voiceManager.prepare(sampleRate, samplesPerBlock);
    }

    void SynthEngine::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) noexcept
    {
        buffer.clear();

        if (pendingResetHardOff_.exchange(false, std::memory_order_relaxed))
        {
            m_voiceManager.allNotesOff(false);
            engineHardOff_.store(false, std::memory_order_relaxed);
        }

        if (engineHardOff_.load(std::memory_order_relaxed))
            return;

        ParameterSnapshot snap{};
        if (m_apvts != nullptr)
            fillParameterSnapshot(*m_apvts, m_guiState, snap);
        m_voiceManager.setBlockParameterSnapshot(snap);

        for (const auto metadata : midiMessages)
        {
            const auto msg = metadata.getMessage();
            const int channel = msg.getChannel();
            if (msg.isNoteOn())
            {
                lastMidiNote_.store(msg.getNoteNumber(), std::memory_order_relaxed);
                lastVelocity_.store(msg.getFloatVelocity(), std::memory_order_relaxed);
                m_voiceManager.noteOn(channel, msg.getNoteNumber(), msg.getFloatVelocity());
            }
            else if (msg.isNoteOff())
            {
                m_voiceManager.noteOff(channel, msg.getNoteNumber(), msg.getFloatVelocity(), true);
            }
            else if (msg.isPitchWheel())
            {
                lastPitchWheel_.store(msg.getPitchWheelValue(), std::memory_order_relaxed);
                m_voiceManager.getSynthesiser().handlePitchWheel(channel, msg.getPitchWheelValue());
            }
            else if (msg.isController())
            {
                lastController_.store(msg.getControllerNumber(), std::memory_order_relaxed);
                lastControllerValue_.store(msg.getControllerValue(), std::memory_order_relaxed);
                m_voiceManager.getSynthesiser().handleController(channel, msg.getControllerNumber(), msg.getControllerValue());
            }
            else if (msg.isAllNotesOff() || msg.isAllSoundOff())
            {
                m_voiceManager.allNotesOff(true);
            }
        }

        const bool anyVoiceRendering = m_voiceManager.hasAnyVoiceRendering();

        if (!anyVoiceRendering)
        {
            if (m_apvts != nullptr)
            {
                meterLevelLeft_.store(-96.0f, std::memory_order_relaxed);
                meterLevelRight_.store(-96.0f, std::memory_order_relaxed);
            }
        }
        else
        {
            m_voiceManager.renderNextBlock(buffer, 0, buffer.getNumSamples());

            // Output Gain and Pan
            if (m_apvts != nullptr)
            {
                const float gainDb = m_apvts->getRawParameterValue(ParameterIDs::outputGain)->load();
                const float pan = m_apvts->getRawParameterValue(ParameterIDs::outputPan)->load();

                // dB
                const float gainLinear = juce::Decibels::decibelsToGain(gainDb, -60.0f);

                // pan
                const float panAngle = (pan + 1.0f) * 0.5f * juce::MathConstants<float>::halfPi;
                const float gainL = gainLinear * std::cos(panAngle);
                const float gainR = gainLinear * std::sin(panAngle);

                if (buffer.getNumChannels() >= 2)
                {
                    buffer.applyGain(0, 0, buffer.getNumSamples(), gainL);
                    buffer.applyGain(1, 0, buffer.getNumSamples(), gainR);
                }
                else if (buffer.getNumChannels() == 1)
                {
                    buffer.applyGain(0, 0, buffer.getNumSamples(), gainLinear);
                }

                const int numSamples = buffer.getNumSamples();
                float peakL = 0.0f;
                float peakR = 0.0f;

                if (buffer.getNumChannels() >= 2)
                {
                    peakL = buffer.getMagnitude(0, 0, numSamples);
                    peakR = buffer.getMagnitude(1, 0, numSamples);
                }
                else if (buffer.getNumChannels() == 1)
                {
                    peakL = buffer.getMagnitude(0, 0, numSamples);
                    peakR = peakL;
                }

                // meter level
                if (buffer.getNumChannels() >= 1)
                {
                    const float meterDbL = juce::Decibels::gainToDecibels(peakL, -96.0f);
                    const float meterDbR = juce::Decibels::gainToDecibels(peakR, -96.0f);
                    meterLevelLeft_.store(meterDbL, std::memory_order_relaxed);
                    meterLevelRight_.store(meterDbR, std::memory_order_relaxed);
                }

                if (juce::jmax(peakL, peakR) > kHardOffClipThresholdLinear)
                {
                    engineHardOff_.store(true, std::memory_order_relaxed);
                    m_voiceManager.allNotesOff(false);
                    buffer.clear();
                    return;
                }
            }
        }
    }

    void SynthEngine::resetEngineHardOff() noexcept
    {
        pendingResetHardOff_.store(true, std::memory_order_relaxed);
    }

    void SynthEngine::processBlock(juce::AudioBuffer<float> &buffer) noexcept
    {
        buffer.clear();
    }

    void SynthEngine::noteOn(int midiChannel, int midiNoteNumber, float velocity) noexcept
    {
        lastMidiNote_.store(midiNoteNumber, std::memory_order_relaxed);
        lastVelocity_.store(velocity, std::memory_order_relaxed);
        m_voiceManager.noteOn(midiChannel, midiNoteNumber, velocity);
    }

    void SynthEngine::noteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) noexcept
    {
        m_voiceManager.noteOff(midiChannel, midiNoteNumber, velocity, allowTailOff);
    }

    void SynthEngine::allNotesOff(bool allowTailOff) noexcept
    {
        m_voiceManager.allNotesOff(allowTailOff);
    }

    void SynthEngine::pitchWheelMoved(int value) noexcept
    {
        lastPitchWheel_.store(value, std::memory_order_relaxed);
        m_voiceManager.getSynthesiser().handlePitchWheel(0, value);
    }

    void SynthEngine::controllerMoved(int controllerNumber, int value) noexcept
    {
        lastController_.store(controllerNumber, std::memory_order_relaxed);
        lastControllerValue_.store(value, std::memory_order_relaxed);
        m_voiceManager.getSynthesiser().handleController(0, controllerNumber, value);
    }

    VoiceManager &SynthEngine::getVoiceManager() noexcept
    {
        return m_voiceManager;
    }

    const VoiceManager &SynthEngine::getVoiceManager() const noexcept
    {
        return m_voiceManager;
    }

    int SynthEngine::getLastMidiNote() const noexcept
    {
        return lastMidiNote_.load(std::memory_order_relaxed);
    }

    float SynthEngine::getLastVelocity() const noexcept
    {
        return lastVelocity_.load(std::memory_order_relaxed);
    }

    int SynthEngine::getLastPitchWheel() const noexcept
    {
        return lastPitchWheel_.load(std::memory_order_relaxed);
    }

    int SynthEngine::getLastController() const noexcept
    {
        return lastController_.load(std::memory_order_relaxed);
    }

    int SynthEngine::getLastControllerValue() const noexcept
    {
        return lastControllerValue_.load(std::memory_order_relaxed);
    }
}
