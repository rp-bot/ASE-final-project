#pragma once

#include <juce_core/juce_core.h>

namespace ParameterIDs
{
    inline const juce::String gain { "GAIN" };
    inline const juce::String cursorX { "CURSOR_X" };
    inline const juce::String cursorY { "CURSOR_Y" };
    inline const juce::String cursorZ { "CURSOR_Z" };

    // Stub per-corner controls for UI wiring (corner 0-7).
    inline juce::String cornerLevel (int cornerIndex)   { return "CORNER" + juce::String (cornerIndex) + "_LEVEL"; }
    inline juce::String cornerDetune (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_DETUNE"; }
    inline juce::String cornerWaveform (int cornerIndex){ return "CORNER" + juce::String (cornerIndex) + "_WAVEFORM"; }
    inline juce::String cornerCoarse (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_COARSE"; }
    inline juce::String cornerFine (int cornerIndex)    { return "CORNER" + juce::String (cornerIndex) + "_FINE"; }
    inline juce::String cornerPan (int cornerIndex)     { return "CORNER" + juce::String (cornerIndex) + "_PAN"; }
}
