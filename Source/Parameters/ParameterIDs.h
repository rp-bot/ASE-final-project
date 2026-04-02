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

    // Per-corner filter page (Hz, normalized resonance, key-track amount, drive)
    inline juce::String cornerFilterCutoff (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_FIL_CUT"; }
    inline juce::String cornerFilterResonance (int cornerIndex) { return "CORNER" + juce::String (cornerIndex) + "_FIL_RES"; }
    inline juce::String cornerFilterKeyTrack (int cornerIndex) { return "CORNER" + juce::String (cornerIndex) + "_FIL_KEY"; }
    inline juce::String cornerFilterDrive (int cornerIndex)   { return "CORNER" + juce::String (cornerIndex) + "_FIL_DRV"; }

    // Per-corner amp page (seconds / level; sustain 0–1; velocity sensitivity 0–1)
    inline juce::String cornerAmpAttack (int cornerIndex)   { return "CORNER" + juce::String (cornerIndex) + "_AMP_ATK"; }
    inline juce::String cornerAmpDecay (int cornerIndex)    { return "CORNER" + juce::String (cornerIndex) + "_AMP_DEC"; }
    inline juce::String cornerAmpSustain (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_AMP_SUS"; }
    inline juce::String cornerAmpRelease (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_AMP_REL"; }
    inline juce::String cornerAmpLevel (int cornerIndex)    { return "CORNER" + juce::String (cornerIndex) + "_AMP_LVL"; }
    inline juce::String cornerAmpVelSens (int cornerIndex)  { return "CORNER" + juce::String (cornerIndex) + "_AMP_VEL"; }
}
