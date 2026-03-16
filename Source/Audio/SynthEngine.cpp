#include "SynthEngine.h"

namespace Audio
{

SynthEngine::SynthEngine(Threading::AtomicGuiState* guiState) noexcept
    : m_guiState(guiState)
{
}

SynthEngine::~SynthEngine()
{
    // Do not clear voices/sounds here; the audio thread may still be in renderNextBlock.
    // Let m_synthesiser destructor clean up when this object is destroyed.
}

void SynthEngine::prepare(double sampleRate, int samplesPerBlock) noexcept
{
    sampleRate_ = sampleRate;
    blockSize_ = samplesPerBlock;

    if (!m_voicesAdded)
    {
        if (m_wavetableBank == nullptr)
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

        m_synthesiser.addSound(new SynthSound());
        m_synthesiser.addVoice(new SynthVoice(m_wavetableBank.get(), m_guiState));
        m_voicesAdded = true;
    }

    m_synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    if (auto* v = m_synthesiser.getVoice(0))
        static_cast<SynthVoice*>(v)->prepare(sampleRate, samplesPerBlock);
}

void SynthEngine::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) noexcept
{
    m_synthesiser.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

void SynthEngine::processBlock(juce::AudioBuffer<float>& buffer) noexcept
{
    buffer.clear();
}

void SynthEngine::noteOn (int midiNoteNumber, float velocity) noexcept
{
    lastMidiNote_.store (midiNoteNumber, std::memory_order_relaxed);
    lastVelocity_.store (velocity, std::memory_order_relaxed);
}

void SynthEngine::noteOff (int midiNoteNumber) noexcept
{
    juce::ignoreUnused (midiNoteNumber);
}

void SynthEngine::allNotesOff() noexcept
{
}

void SynthEngine::pitchWheelMoved (int value) noexcept
{
    lastPitchWheel_.store (value, std::memory_order_relaxed);
}

void SynthEngine::controllerMoved (int controllerNumber, int value) noexcept
{
    lastController_.store (controllerNumber, std::memory_order_relaxed);
    lastControllerValue_.store (value, std::memory_order_relaxed);
}

int SynthEngine::getLastMidiNote() const noexcept
{
    return lastMidiNote_.load (std::memory_order_relaxed);
}

float SynthEngine::getLastVelocity() const noexcept
{
    return lastVelocity_.load (std::memory_order_relaxed);
}

int SynthEngine::getLastPitchWheel() const noexcept
{
    return lastPitchWheel_.load (std::memory_order_relaxed);
}

int SynthEngine::getLastController() const noexcept
{
    return lastController_.load (std::memory_order_relaxed);
}

int SynthEngine::getLastControllerValue() const noexcept
{
    return lastControllerValue_.load (std::memory_order_relaxed);
}

} // namespace Audio
