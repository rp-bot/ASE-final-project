#include "WavetableBank.h"
#include <iostream>

namespace DSP
{

//WAVE GENERATORS
void WavetableBank::generateSineWave(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = std::sin(2.0f * juce::MathConstants<float>::pi * i / numSamples);
        buffer.setSample(0, i, sample);
    }
}
void WavetableBank::generateSawtoothWave(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 2.0f * (i / static_cast<float>(numSamples)) - 1.0f;
        buffer.setSample(0, i, sample);
    }
}
void WavetableBank::generateSquareWave(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = (i < numSamples / 2) ? 1.0f : -1.0f;
        buffer.setSample(0, i, sample);
    }
}

void WavetableBank::generateTriangleWave(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    for (int i = 0; i < numSamples; ++i)
    {
        float sample = 2.0f * std::abs(2.0f * (i / static_cast<float>(numSamples)) - 1.0f) - 1.0f;
        buffer.setSample(0, i, sample);
    }
}

//LOAD WAVETABLE
void WavetableBank::loadWavetable(const juce::File& file)
{
    // Placeholder for loading wavetable from file
    std::cout << "Loading wavetable from: " << file.getFullPathName() << std::endl;
}


//GENERATE DEFAULT WAVETABLE
void WavetableBank::generateDefaultWavetable(uint index, WaveformType type, int tableSize)
{
    juce::AudioBuffer<float> buffer(1, tableSize);
    switch (type)
    {
        case WaveformType::Sine:
            generateSineWave(buffer);
            break;
        case WaveformType::Sawtooth:
            generateSawtoothWave(buffer);
            break;
        case WaveformType::Square:
            generateSquareWave(buffer);
            break;
        case WaveformType::Triangle:
            generateTriangleWave(buffer);
            break;
    }
    jassert(index >= 0 && index < wavetables.size());
    wavetables[index] = std::move(buffer);
}

//GET A WAVETABLE
const juce::AudioBuffer<float>& WavetableBank::getWavetable(int index) const
{
    jassert(index >= 0 && index < wavetables.size());
    return wavetables[index];
}

//GET THE NUMBER OF WAVETABLES
int WavetableBank::getNumWavetables() const
{
    return wavetables.size();
}

//ADD A WAVETABLE 
void WavetableBank::addWavetable(juce::AudioBuffer<float>&& buffer)
{
    wavetables.push_back(std::move(buffer));
}

//CLEAR ALL WAVETABLES
void WavetableBank::clear()
    {
    wavetables.clear(); 
    }
}