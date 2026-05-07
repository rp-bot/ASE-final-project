#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "ParameterIDs.h"

class ParameterLayout
{
public:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::cursorX,
                                                                 "Cursor X",
                                                                 juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
                                                                 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::cursorY,
                                                                 "Cursor Y",
                                                                 juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
                                                                 0.5f));
        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::cursorZ,
                                                                 "Cursor Z",
                                                                 juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
                                                                 0.5f));

        layout.add (std::make_unique<juce::AudioParameterBool> (ParameterIDs::cubeZeroG,
                                                                "Zero G",
                                                                false));

        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::cameraZoom,
                                                                 "Camera Zoom",
                                                                 juce::NormalisableRange<float> (1.0f, 20.0f, 0.01f),
                                                                 5.0f));
        layout.add (std::make_unique<juce::AudioParameterBool> (ParameterIDs::gizmoVisible,
                                                                "Gizmo Visible",
                                                                true));
        
        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::outputGain, 
                                                                "Output Gain",
                                                                juce::NormalisableRange<float> (-60.0f, 6.0f, 0.1f), 
                                                                0.0f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (ParameterIDs::outputPan, 
                                                                "Pan",
                                                                juce::NormalisableRange<float> (-1.0f, 1.0f, 0.01f), 
                                                                0.0f));

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

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerFilterCutoff (corner),
                cornerName + "Filter Cutoff",
                juce::NormalisableRange<float> (20.0f, 20000.0f, 1.0f, 0.25f),
                4000.0f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerFilterResonance (corner),
                cornerName + "Filter Resonance",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                0.2f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerFilterKeyTrack (corner),
                cornerName + "Filter Key Track",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                0.5f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerFilterDrive (corner),
                cornerName + "Filter Drive",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                0.0f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerAmpAttack (corner),
                cornerName + "Amp Attack",
                juce::NormalisableRange<float> (0.005f, 10.0f, 0.001f, 0.4f),
                0.01f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerAmpDecay (corner),
                cornerName + "Amp Decay",
                juce::NormalisableRange<float> (0.005f, 10.0f, 0.001f, 0.4f),
                0.1f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerAmpSustain (corner),
                cornerName + "Amp Sustain",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                0.7f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerAmpRelease (corner),
                cornerName + "Amp Release",
                juce::NormalisableRange<float> (0.005f, 10.0f, 0.001f, 0.4f),
                0.3f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerAmpLevel (corner),
                cornerName + "Amp Level",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                1.0f));

            layout.add (std::make_unique<juce::AudioParameterFloat> (
                ParameterIDs::cornerAmpVelSens (corner),
                cornerName + "Amp Vel Sens",
                juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
                0.5f));

            layout.add (std::make_unique<juce::AudioParameterBool> (
                ParameterIDs::oscFilterEnabled (corner),
                cornerName + "Filter Master Enable",
                false));

            layout.add (std::make_unique<juce::AudioParameterBool> (
                ParameterIDs::oscAmpEnabled (corner),
                cornerName + "Amp Master Enable",
                false));
        }

        // Master filter (ranges/defaults match per-corner filter parameters).
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterFilterCutoff,
            "Master Filter Cutoff",
            juce::NormalisableRange<float> (20.0f, 20000.0f, 1.0f, 0.25f),
            4000.0f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterFilterResonance,
            "Master Filter Resonance",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
            0.2f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterFilterKeyTrack,
            "Master Filter Key Track",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
            0.5f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterFilterDrive,
            "Master Filter Drive",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
            0.0f));

        // Master amp envelope (ranges/defaults match per-corner amp parameters).
        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterAmpAttack,
            "Master Amp Attack",
            juce::NormalisableRange<float> (0.005f, 10.0f, 0.001f, 0.4f),
            0.01f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterAmpDecay,
            "Master Amp Decay",
            juce::NormalisableRange<float> (0.005f, 10.0f, 0.001f, 0.4f),
            0.1f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterAmpSustain,
            "Master Amp Sustain",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
            0.7f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterAmpRelease,
            "Master Amp Release",
            juce::NormalisableRange<float> (0.005f, 10.0f, 0.001f, 0.4f),
            0.3f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterAmpLevel,
            "Master Amp Level",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
            1.0f));

        layout.add (std::make_unique<juce::AudioParameterFloat> (
            ParameterIDs::masterAmpVelSens,
            "Master Amp Vel Sens",
            juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f),
            0.5f));

        return layout;
    }
};
