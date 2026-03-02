#pragma once

#include <juce_core/juce_core.h>

namespace ParameterIDs
{
    inline const juce::String gain { "GAIN" };

    // Stub per-corner controls for UI wiring (corner 0-7).
    inline juce::String cornerLevel (int cornerIndex)   { return "CORNER" + juce::String (cornerIndex) + "_LEVEL"; }
    inline juce::String cornerDetune (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_DETUNE"; }
    inline juce::String cornerWaveform (int cornerIndex){ return "CORNER" + juce::String (cornerIndex) + "_WAVEFORM"; }
    inline juce::String cornerCoarse (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_COARSE"; }
    inline juce::String cornerFine (int cornerIndex)    { return "CORNER" + juce::String (cornerIndex) + "_FINE"; }
    inline juce::String cornerPan (int cornerIndex)     { return "CORNER" + juce::String (cornerIndex) + "_PAN"; }
}
