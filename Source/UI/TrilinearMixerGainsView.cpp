// TRILINEAR MIXER VISUALIZATION (see TrilinearMixerGainsView.h for removal steps)
#include "TrilinearMixerGainsView.h"

namespace UI
{
TrilinearMixerGainsView::TrilinearMixerGainsView()
{
    addAndMakeVisible (m_cube);
    m_cube.setCursorPosition (m_x, m_y, m_z);
}

void TrilinearMixerGainsView::setPosition (float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;
    m_cube.setCursorPosition (x, y, z);
}

void TrilinearMixerGainsView::resized()
{
    m_cube.setBounds (getLocalBounds());
}
} // namespace UI
