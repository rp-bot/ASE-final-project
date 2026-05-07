#include "WavetableOscillator.h"
#include "WavetableBank.h"
#include <juce_core/juce_core.h>
#include <algorithm>

namespace DSP
{

    WavetableOscillator::WavetableOscillator(WavetableBank *wavetableBank)
        : m_wavetableBank(wavetableBank)
    {
    }

    float WavetableOscillator::processSample()
    {
        // Zero sample rate: don't process (issue: handle gracefully)
        if (m_sampleRate <= 0.0)
            return 0.0f;

        float sample = interpolateWavetable(m_phase);
        m_phase += m_phaseIncrement;
        if (m_phase >= 1.0f)
            m_phase -= 1.0f;
        return sample;
    }

    void WavetableOscillator::processBlock(float *dest, int numSamples)
    {
        if (m_sampleRate <= 0.0 || dest == nullptr || numSamples <= 0)
            return;

        const float inc = m_phaseIncrement;
        float phase = m_phase;

        for (int i = 0; i < numSamples; ++i)
        {
            dest[i] = interpolateWavetable(phase);
            phase += inc;
            if (phase >= 1.0f)
                phase -= 1.0f;
        }

        m_phase = phase;
    }

    float WavetableOscillator::interpolateWavetable(float phase) const
    {
        jassert(m_wavetableBank != nullptr);
        if (m_wavetableBank == nullptr)
            return 0.0f;

        const int numWavetables = m_wavetableBank->getNumWavetables();
        if (numWavetables == 0)
            return 0.0f;

        const int safeIndex = std::clamp(m_wavetableIndex, 0, numWavetables - 1);
        const auto &wavetable = m_wavetableBank->getWavetable(safeIndex);
        const int tableSize = wavetable.getNumSamples();
        if (tableSize == 0)
            return 0.0f;

        const float position = phase * static_cast<float>(tableSize);
        const int index0 = static_cast<int>(position);
        const int index1 = (index0 + 1) % tableSize;
        const float fraction = position - static_cast<float>(index0);

        const float sample0 = wavetable.getSample(0, index0);
        const float sample1 = wavetable.getSample(0, index1);
        return sample0 + fraction * (sample1 - sample0);
    }

    void WavetableOscillator::setWavetable(int index)
    {
        jassert(m_wavetableBank != nullptr);
        if (m_wavetableBank == nullptr)
            return;
        const int numWavetables = m_wavetableBank->getNumWavetables();
        m_wavetableIndex = (numWavetables > 0) ? std::clamp(index, 0, numWavetables - 1) : 0;
    }

    int WavetableOscillator::getCurrentWavetableIndex() const
    {
        return m_wavetableIndex;
    }

    float WavetableOscillator::getPhase() const
    {
        return m_phase;
    }

    void WavetableOscillator::setWavetableBank(WavetableBank *bank)
    {
        m_wavetableBank = bank;
    }

    void WavetableOscillator::setPhase(float phase)
    {
        m_phase = phase;
    }

    void WavetableOscillator::setFrequency(float frequencyHz)
    {
        m_frequency = frequencyHz;
        if (m_sampleRate > 0.0)
            m_phaseIncrement = m_frequency / static_cast<float>(m_sampleRate);
    }

    void WavetableOscillator::prepare(double sampleRate)
    {
        m_sampleRate = sampleRate;
        setFrequency(m_frequency);
    }

    void WavetableOscillator::reset()
    {
        m_phase = 0.0f;
    }

}
