#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ParameterLayout.h"

class ParameterManager
{
public:
    ParameterManager (juce::AudioProcessor& processor)
        : apvts (processor, nullptr, "PARAMETERS", ParameterLayout::createParameterLayout())
    {
    }

    float getGain() const { return *apvts.getRawParameterValue (ParameterIDs::gain); }
    
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
};
