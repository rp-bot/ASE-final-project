#include "LabeledPanel.h"

namespace UI
{
LabeledPanel::LabeledPanel (juce::String text)
{
    titleLabel.setText (text, juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (titleLabel);
}

void LabeledPanel::setText (const juce::String& text)
{
    titleLabel.setText (text, juce::dontSendNotification);
}

void LabeledPanel::paint (juce::Graphics& g)
{
    if (filledBackground)
    {
        g.setColour (juce::Colours::darkgrey.withAlpha (0.25f));
        g.fillRect (getLocalBounds());
    }

    g.setColour (juce::Colours::grey.withAlpha (0.6f));
    g.drawRect (getLocalBounds(), 1);
}

void LabeledPanel::resized()
{
    titleLabel.setBounds (getLocalBounds().reduced (6));
}
} // namespace UI
