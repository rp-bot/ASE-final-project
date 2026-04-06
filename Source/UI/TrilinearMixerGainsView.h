#pragma once

// TRILINEAR MIXER VISUALIZATION: Optional UI. To remove: delete this file and .cpp,
// remove TrilinearMixerGainsView from UI/CMakeLists.txt, and remove all blocks
// marked "TRILINEAR MIXER VISUALIZATION" in CenterControlPanel.

#include <juce_gui_basics/juce_gui_basics.h>
#include "TrilinearCube.h"

namespace UI
{
/**
 * Displays a simple OpenGL trilinear cube view.
 * Call setPosition(x, y, z) to update the cube cursor position.
 */
class TrilinearMixerGainsView : public juce::Component
{
public:
    TrilinearMixerGainsView();

    void setPosition (float x, float y, float z);
    void resized() override;

private:
    float m_x { 0.5f };
    float m_y { 0.5f };
    float m_z { 0.5f };
    TrilinearCube m_cube;
};

} // namespace UI
