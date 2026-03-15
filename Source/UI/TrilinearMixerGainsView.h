#pragma once

// TRILINEAR MIXER VISUALIZATION: Optional UI. To remove: delete this file and .cpp,
// remove TrilinearMixerGainsView from UI/CMakeLists.txt, and remove all blocks
// marked "TRILINEAR MIXER VISUALIZATION" in CenterControlPanel.

#include <juce_gui_basics/juce_gui_basics.h>
#include <array>

namespace UI
{
/**
 * Displays the 8 trilinear mixer gains (one per cube corner) as vertical bars.
 * Call setPosition(x, y, z) to update gains from Utils::trilinearWeights.
 */
class TrilinearMixerGainsView : public juce::Component
{
public:
    TrilinearMixerGainsView();

    void setPosition (float x, float y, float z);
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    std::array<float, 8> m_gains {};
    float m_x { 0.5f };
    float m_y { 0.5f };
    float m_z { 0.5f };
    static std::array<juce::Colour, 8> getCornerColours();
};

} // namespace UI
