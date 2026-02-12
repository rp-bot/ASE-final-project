#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ParameterIDs.h"

class ParameterLayout
{
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::gain,
                                                                 "Gain",
                                                                 juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                                                                 0.5f));

        return layout;
    }
};
