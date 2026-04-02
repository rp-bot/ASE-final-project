#include "ParameterSnapshot.h"

#include <cmath>

#include "../Parameters/ParameterIDs.h"
#include "../Threading/AtomicGuiState.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace Audio
{
    namespace
    {
        float readFloat (juce::AudioProcessorValueTreeState& apvts, const juce::String& id, float fallback) noexcept
        {
            if (auto* p = apvts.getRawParameterValue (id))
                return p->load();
            return fallback;
        }

        int readChoiceIndex (juce::AudioProcessorValueTreeState& apvts, const juce::String& id,
                             int numChoices, int fallback) noexcept
        {
            if (numChoices < 2)
                return fallback;
            if (auto* raw = apvts.getRawParameterValue (id))
            {
                const float v = raw->load();
                if (v >= 0.0f && v <= 1.0f + 1.0e-3f)
                {
                    const int idx =
                        static_cast<int> (std::lround (v * static_cast<float> (numChoices - 1)));
                    return juce::jlimit (0, numChoices - 1, idx);
                }
                return juce::jlimit (0, numChoices - 1, static_cast<int> (std::lround (v)));
            }
            return fallback;
        }
    } // namespace

    void fillParameterSnapshot (juce::AudioProcessorValueTreeState& apvts,
                                const Threading::AtomicGuiState* guiState,
                                ParameterSnapshot& out) noexcept
    {
        if (guiState != nullptr)
        {
            out.cursor = guiState->getCursorPosition();
            out.trajectoryActive = guiState->isTrajectoryActive();
        }
        else
        {
            out.cursor = glm::vec3 (readFloat (apvts, ParameterIDs::cursorX, 0.5f),
                                    readFloat (apvts, ParameterIDs::cursorY, 0.5f),
                                    readFloat (apvts, ParameterIDs::cursorZ, 0.5f));
            out.trajectoryActive = false;
        }

        for (int i = 0; i < NUM_OSCS; ++i)
        {
            auto& o = out.osc[static_cast<size_t> (i)];
            o.waveform = readChoiceIndex (apvts, ParameterIDs::cornerWaveform (i), 4, 0);
            o.level = readFloat (apvts, ParameterIDs::cornerLevel (i), 0.75f);
            o.detune = readFloat (apvts, ParameterIDs::cornerDetune (i), 0.0f);
            o.coarse = readFloat (apvts, ParameterIDs::cornerCoarse (i), 0.0f);
            o.fine = readFloat (apvts, ParameterIDs::cornerFine (i), 0.0f);
            o.pan = readFloat (apvts, ParameterIDs::cornerPan (i), 0.0f);

            o.filterCutoffHz = readFloat (apvts, ParameterIDs::cornerFilterCutoff (i), 4000.0f);
            o.filterResonance = readFloat (apvts, ParameterIDs::cornerFilterResonance (i), 0.2f);
            o.filterKeyTrack = readFloat (apvts, ParameterIDs::cornerFilterKeyTrack (i), 0.5f);
            o.filterDrive = readFloat (apvts, ParameterIDs::cornerFilterDrive (i), 0.0f);

            o.ampAttack = readFloat (apvts, ParameterIDs::cornerAmpAttack (i), 0.01f);
            o.ampDecay = readFloat (apvts, ParameterIDs::cornerAmpDecay (i), 0.1f);
            o.ampSustain = readFloat (apvts, ParameterIDs::cornerAmpSustain (i), 0.7f);
            o.ampRelease = readFloat (apvts, ParameterIDs::cornerAmpRelease (i), 0.3f);
            o.ampLevel = readFloat (apvts, ParameterIDs::cornerAmpLevel (i), 1.0f);
            o.ampVelSens = readFloat (apvts, ParameterIDs::cornerAmpVelSens (i), 0.5f);
        }
    }
}
