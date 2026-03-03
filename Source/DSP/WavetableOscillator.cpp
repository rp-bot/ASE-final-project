#include "WavetableOscillator.h"
#include <iostream>

float WavetableOscillator::processSample()
{

float sample = interpolateWavetable(m_phase);
m_phase += m_phaseIncrement;
if (m_phase >= 1.0f)
    m_phase -= 1.0f;  // Wrap to [0, 1)
return sample;

}



float WavetableOscillator::interpolateWavetable(float phase) const
{
    const auto& wavetable = m_wavetableBank->getWavetable(m_wavetableIndex);
    int tableSize = wavetable.getNumSamples();
    
    float position = phase * tableSize;
    int index0 = static_cast<int>(position);
    int index1 = (index0 + 1) % tableSize;  // Wrap around
    float fraction = position - index0;
    
    float sample0 = wavetable.getSample(0, index0);
    float sample1 = wavetable.getSample(0, index1);
    
    return sample0 + fraction * (sample1 - sample0);
}

void WavetableOscillator::setFrequency(float frequencyHz)
{
    m_frequency = frequencyHz;
    if (m_sampleRate > 0.0)
        m_phaseIncrement = m_frequency / static_cast<float>(m_sampleRate);
}

