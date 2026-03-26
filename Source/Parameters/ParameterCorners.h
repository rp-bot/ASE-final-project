#pragma once
#include "ParameterIDs.h"
#include "DSP/WavetableBank.h"

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>


// struct for storing corner params
struct CornerParams
{
    DSP::WaveformType waveform = DSP::WaveformType::Sine; // default to sine
    // int waveform = 0; // order from ParameterLayout: "Sine", "Saw", "Square", "Triangle"
    float level = 0.0f;
    float detune  = 0.0f;
    float coarse = 0.0f;
    float fine = 0.0f;
    float pan = 0.0f;
};

/* from WavetableBank.h: (maintains same order as UI)
    enum class WaveformType
    {
    Sine,
    Sawtooth,
    Square,
    Triangle
    };
*/

// class for retrieving corner parameters
class CornerParamReader
{
public:

    // loop thorugh each corner & retrieve parameter info from UI
    CornerParamReader(juce::AudioProcessorValueTreeState& apvts) {
        for (int i = 0; i < 8; ++i) { // NOTE: 8 is hardcoded right now. Assuming we are not changing the number of synths we have
            m_level[i]    = apvts.getRawParameterValue(ParameterIDs::cornerLevel(i));
            m_detune[i]   = apvts.getRawParameterValue(ParameterIDs::cornerDetune(i));
            m_waveform[i] = apvts.getRawParameterValue(ParameterIDs::cornerWaveform(i));
            m_coarse[i]   = apvts.getRawParameterValue(ParameterIDs::cornerCoarse(i));
            m_fine[i]     = apvts.getRawParameterValue(ParameterIDs::cornerFine(i));
            m_pan[i]      = apvts.getRawParameterValue(ParameterIDs::cornerPan(i));
        }
    }

    // get corner param of specific corner (0-7)
    // load(): lock free atomic ptr read
    CornerParams getCorner(int i) const
    {
        return {
            static_cast<DSP::WaveformType>(static_cast<int>(m_waveform[i]->load())),
            m_level[i]->load(),
            m_detune[i]->load(),
            m_coarse[i]->load(),
            m_fine[i]->load(),
            m_pan[i]->load()
        };
    }

private:
    std::array<std::atomic<float>*, 8> m_level;
    std::array<std::atomic<float>*, 8> m_detune;
    std::array<std::atomic<float>*, 8> m_waveform; // keeping this to ensure lock free reading
    std::array<std::atomic<float>*, 8> m_coarse;
    std::array<std::atomic<float>*, 8> m_fine;
    std::array<std::atomic<float>*, 8> m_pan;
};