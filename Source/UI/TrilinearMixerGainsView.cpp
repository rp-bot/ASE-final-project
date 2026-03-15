// TRILINEAR MIXER VISUALIZATION (see TrilinearMixerGainsView.h for removal steps)
#include "TrilinearMixerGainsView.h"
#include "../Utils/Math3D.h"

namespace UI
{
TrilinearMixerGainsView::TrilinearMixerGainsView()
{
    m_gains = Utils::trilinearWeights (0.5f, 0.5f, 0.5f);
}

std::array<juce::Colour, 8> TrilinearMixerGainsView::getCornerColours()
{
    return { juce::Colour::fromRGB (231, 76, 60),   // red     - corner 0
             juce::Colour::fromRGB (230, 126, 34),  // orange  - corner 1
             juce::Colour::fromRGB (241, 196, 15),   // yellow  - corner 2
             juce::Colour::fromRGB (46, 204, 113),  // green   - corner 3
             juce::Colour::fromRGB (26, 188, 156),  // teal    - corner 4
             juce::Colour::fromRGB (52, 152, 219),  // blue    - corner 5
             juce::Colour::fromRGB (155, 89, 182),  // purple  - corner 6
             juce::Colour::fromRGB (236, 112, 173) }; // pink   - corner 7
}

void TrilinearMixerGainsView::setPosition (float x, float y, float z)
{
    m_x = x;
    m_y = y;
    m_z = z;
    m_gains = Utils::trilinearWeights (x, y, z);
    repaint();
}

void TrilinearMixerGainsView::paint (juce::Graphics& g)
{
    const auto colours = getCornerColours();
    const auto bounds = getLocalBounds().reduced (2);
    if (bounds.isEmpty())
        return;

    const int numBars = 8;
    const int gap = 2;
    const int totalGap = gap * (numBars - 1);
    const int barWidth = (bounds.getWidth() - totalGap) / numBars;

    for (int i = 0; i < numBars; ++i)
    {
        const int x = bounds.getX() + i * (barWidth + gap);
        const float gain = m_gains[static_cast<size_t> (i)];
        const int barHeight = juce::jmax (2, juce::roundToInt (bounds.getHeight() * gain));
        const int y = bounds.getBottom() - barHeight;

        g.setColour (colours[static_cast<size_t> (i)].withAlpha (0.85f));
        g.fillRoundedRectangle (static_cast<float> (x), static_cast<float> (y),
                               static_cast<float> (barWidth), static_cast<float> (barHeight), 2.0f);
        g.setColour (colours[static_cast<size_t> (i)].brighter (0.3f));
        g.drawRoundedRectangle (static_cast<float> (x), static_cast<float> (y),
                                static_cast<float> (barWidth), static_cast<float> (barHeight), 2.0f, 0.5f);
    }
}

void TrilinearMixerGainsView::resized()
{
}
} // namespace UI
