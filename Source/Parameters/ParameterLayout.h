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

        for (int corner = 0; corner < 8; ++corner)
        {
            const auto cornerName = "Corner " + juce::String (corner + 1) + " ";

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerLevel (corner),
                cornerName + "Level",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                0.75f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerDetune (corner),
                cornerName + "Detune",
                juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f),
                0.0f));

            layout.add (std::make_unique<juce::AudioParameterChoice> (
                ParameterIDs::cornerWaveform (corner),
                cornerName + "Waveform",
                juce::StringArray { "Sine", "Saw", "Square", "Triangle" },
                0));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerCoarse (corner),
                cornerName + "Coarse",
                juce::NormalisableRange<float> (-24.0f, 24.0f, 1.0f),
                0.0f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerFine (corner),
                cornerName + "Fine",
                juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f),
                0.0f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerPan (corner),
                cornerName + "Pan",
                juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f),
                0.0f));
        }

        return layout;
    }
};
