#pragma once

#include <juce_core/juce_core.h>

namespace ParameterIDs
{
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

    // Per-corner filter/amp lookup-by-index helpers (single source of truth shared
    // by OscillatorModuleComponent and MasterControls).
    inline juce::String cornerFilterById (int cornerIndex, int paramIndex) {
        switch (paramIndex)
        {
            case 0: return cornerFilterCutoff (cornerIndex);
            case 1: return cornerFilterResonance (cornerIndex);
            case 2: return cornerFilterKeyTrack (cornerIndex);
            case 3: return cornerFilterDrive (cornerIndex);
            default: break;
        }
        jassertfalse;
        return {};
    }

    inline juce::String cornerAmpById (int cornerIndex, int paramIndex) {
        switch (paramIndex)
        {
            case 0: return cornerAmpAttack (cornerIndex);
            case 1: return cornerAmpDecay (cornerIndex);
            case 2: return cornerAmpSustain (cornerIndex);
            case 3: return cornerAmpRelease (cornerIndex);
            case 4: return cornerAmpLevel (cornerIndex);
            case 5: return cornerAmpVelSens (cornerIndex);
            default: break;
        }
        jassertfalse;
        return {};
    }

    // Master filter/envelope controls
    inline const juce::String masterFilterCutoff    { "master_filter_cutoff" };
    inline const juce::String masterFilterResonance { "master_filter_resonance" };
    inline const juce::String masterFilterKeyTrack  { "master_filter_keytrack" };
    inline const juce::String masterFilterDrive     { "master_filter_drive" };

    inline const juce::String masterAmpAttack   { "master_amp_attack" };
    inline const juce::String masterAmpDecay    { "master_amp_decay" };
    inline const juce::String masterAmpSustain  { "master_amp_sustain" };
    inline const juce::String masterAmpRelease  { "master_amp_release" };
    inline const juce::String masterAmpLevel    { "master_amp_level" };
    inline const juce::String masterAmpVelSens  { "master_amp_velsens" };

    inline juce::String masterFilter (int paramIndex) {
        switch (paramIndex)
        {
            case 0: return masterFilterCutoff;
            case 1: return masterFilterResonance;
            case 2: return masterFilterKeyTrack;
            case 3: return masterFilterDrive;
            default: break;
        }
        jassertfalse;
        return {};
    }

    inline juce::String masterAmp (int paramIndex) {
        switch (paramIndex)
        {
            case 0: return masterAmpAttack;
            case 1: return masterAmpDecay;
            case 2: return masterAmpSustain;
            case 3: return masterAmpRelease;
            case 4: return masterAmpLevel;
            case 5: return masterAmpVelSens;
            default: break;
        }
        jassertfalse;
        return {};
    }

    // Per-corner enable toggles wired up by MasterControls; control which
    // oscillators receive master-knob writes.
    inline juce::String oscFilterEnabled (int cornerIndex) { return "osc_filter_" + juce::String (cornerIndex) + "_enabled"; }
    inline juce::String oscAmpEnabled    (int cornerIndex) { return "osc_amp_"    + juce::String (cornerIndex) + "_enabled"; }


    // Global controls
    inline constexpr const char* outputGain = "outputGain";
    inline constexpr const char* outputPan  = "outputPan";
}
