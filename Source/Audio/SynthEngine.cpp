#include "SynthEngine.h"

namespace Audio
{

void SynthEngine::prepare (double sampleRate, int samplesPerBlock) noexcept
{
    sampleRate_ = sampleRate;
    blockSize_ = samplesPerBlock;
    juce::ignoreUnused (sampleRate_, blockSize_);
}

void SynthEngine::processBlock (juce::AudioBuffer<float>& buffer) noexcept
{
    // For now, produce silence. Future work will route oscillators, filters, etc.
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
