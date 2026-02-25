#pragma once
#include <juce_dsp/juce_dsp.h>
namespace DSP
{
    enum class WaveformType
    {
        Sine,
        Sawtooth,
        Square,
        Triangle    
    };

    class WavetableBank
    {
    public:
        WavetableBank();
        ~WavetableBank();
        
        // Load a wavetable from a file (future: .wav import)
        // For now, generate simple waveforms programmatically
        void loadWavetable(const juce::File& file);
        
        // Generate a default wavetable (sine, saw, square, triangle)
        void generateDefaultWavetable(uint index, WaveformType type, int tableSize = 2048);
        // Get wavetable by index
        const juce::AudioBuffer<float>& getWavetable(int index) const;
        
        // Get number of loaded wavetables
        int getNumWavetables() const;
        // Clear all wavetables
        void clear();
        
        // Add a wavetable from an AudioBuffer
        void addWavetable(juce::AudioBuffer<float>&& buffer);

        std::vector<juce::AudioBuffer<float>> wavetables;

        void generateSineWave(juce::AudioBuffer<float>& buffer);
        void generateSawtoothWave(juce::AudioBuffer<float>& buffer);
        void generateSquareWave(juce::AudioBuffer<float>& buffer);
        void generateTriangleWave(juce::AudioBuffer<float>& buffer);
    };



}