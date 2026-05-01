#include "TopBar.h"

namespace UI
{

void LogoPlaceholder::paint (juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced (2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.12f));
    g.fillRoundedRectangle (r, 6.0f);
    g.setColour (juce::Colours::white.withAlpha (0.35f));
    g.drawRoundedRectangle (r, 6.0f, 1.0f);
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.setFont (juce::Font (16.0f, juce::Font::bold));
    g.drawText ("VS", getLocalBounds(), juce::Justification::centred, false);
}

TopBar::TopBar (juce::AudioProcessorValueTreeState& apvts,
                std::function<void()> onResetEngineHardOff)
    : outputSection (apvts, std::move (onResetEngineHardOff))
{
    addAndMakeVisible (logoPlaceholder);
    titleLabel.setText ("Volumetric Synth", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);
    addAndMakeVisible (outputSection);
}

void TopBar::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::grey.withAlpha (0.35f));
    g.drawHorizontalLine (getHeight() - 1, 0.0f, static_cast<float> (getWidth()));
}

void TopBar::resized()
{
    auto b = getLocalBounds().reduced (4, 4);

    auto right = b.removeFromRight (juce::jmax (kOutputMinWidth, b.getWidth() / 3));
    b.removeFromRight (8);

    logoPlaceholder.setBounds (b.removeFromLeft (kLogoSide).withSizeKeepingCentre (kLogoSide, kLogoSide));
    b.removeFromLeft (12);

    titleLabel.setBounds (b);
    outputSection.setBounds (right);
}

void TopBar::setMeterLevels (float leftDb, float rightDb)
{
    outputSection.setMeterLevels (leftDb, rightDb);
}

} // namespace UI
