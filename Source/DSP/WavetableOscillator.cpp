#include "WavetableOscillator.h" // our oscillator class definition
#include "WavetableBank.h"       // holds the collection of stored waveforms
#include "UnisonOscillator.h"    // used for layering multiple oscillators together
#include <iostream>              // standard I/O (for debugging)

namespace DSP { // groups everything under the DSP (Digital Signal Processing) namespace

// Called once per audio sample to produce the next output value
float WavetableOscillator::processSample()
{

float sample = interpolateWavetable(m_phase); // read the waveform value at the current position
m_phase += m_phaseIncrement;                  // advance the position through the waveform
if (m_phase >= 1.0f)
    m_phase -= 1.0f;  // if we've gone past the end of one cycle, wrap back to the start
return sample;        // return the computed audio sample value

}

// Reads a smooth waveform value at a given phase (position 0.0–1.0 through the cycle)
//using phase to basically iterate through the buffer has many advantages to iterating through the buffer manually with a for loop or whatever
//so that's what we do here
float WavetableOscillator::interpolateWavetable(float phase) const
{
    const auto& wavetable = m_wavetableBank->getWavetable(m_wavetableIndex); // grab the selected waveform (e.g. sine, saw, square)
    int tableSize = wavetable.getNumSamples();                                // how many stored samples are in this waveform
    
    float position = phase * tableSize; // convert 0–1 phase into an actual index position in the table
    int index0 = static_cast<int>(position);       // the sample just before our position (floor)
    int index1 = (index0 + 1) % tableSize;         // the sample just after, wrapping around at the end
    float fraction = position - index0;             // how far we are between those two samples (0.0–1.0)
    
    float sample0 = wavetable.getSample(0, index0); // value of the sample before
    float sample1 = wavetable.getSample(0, index1); // value of the sample after
    
    return sample0 + fraction * (sample1 - sample0); // blend between the two samples for a smooth result (linear interpolation)
}

// Updates the oscillator's pitch by computing how fast to step through the waveform
void WavetableOscillator::setFrequency(float frequencyHz)
{
    m_frequency = frequencyHz; // store the desired frequency in Hz (e.g. 440 = A4)
    if (m_sampleRate > 0.0)
        m_phaseIncrement = m_frequency / static_cast<float>(m_sampleRate); // how much to advance per sample: higher freq = bigger steps
}

}
/*
currently wondering:
    how to read in oscillator buffer
    how to play back sound
        is the "return sample0 + fraction * (sample1 - sample0);" line the output?
        how to get the right waveform type from Oscillator.cpp/.h



*/



