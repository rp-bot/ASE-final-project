#include "SynthEngine.h"

namespace Audio
{
    SynthEngine::SynthEngine(Threading::AtomicGuiState* guiState) noexcept
        : m_voiceManager(guiState)
    {
    }

    void SynthEngine::prepare(double sampleRate, int samplesPerBlock) noexcept
    {
        m_sampleRate = sampleRate;
        m_blockSize = samplesPerBlock;
        m_voiceManager.prepare(sampleRate, samplesPerBlock);
    }

    void SynthEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) noexcept
    {
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

        buffer.clear();
        m_voiceManager.renderNextBlock(buffer, 0, buffer.getNumSamples());
    }

    void SynthEngine::processBlock(juce::AudioBuffer<float>& buffer) noexcept
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

    VoiceManager& SynthEngine::getVoiceManager() noexcept
    {
        return m_voiceManager;
    }

    const VoiceManager& SynthEngine::getVoiceManager() const noexcept
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
