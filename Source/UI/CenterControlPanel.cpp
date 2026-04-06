#include "CenterControlPanel.h"
#include "../Parameters/ParameterIDs.h"

namespace UI
{
CenterControlPanel::CenterControlPanel (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    configureRotarySlider (xSlider, xLabel, "X");
    configureRotarySlider (ySlider, yLabel, "Y");
    configureRotarySlider (zSlider, zLabel, "Z");
    configureRotarySlider (gainSlider, gainLabel, "Gain");

    for (auto* slider : { &xSlider, &ySlider, &zSlider })
    {
        slider->setRange (0.0, 1.0, 0.001);
        slider->onValueChange = [this] { updateCursorFromSliders(); };
        addAndMakeVisible (*slider);
    }

    addAndMakeVisible (xLabel);
    addAndMakeVisible (yLabel);
    addAndMakeVisible (zLabel);
    addAndMakeVisible (gainSlider);
    addAndMakeVisible (gainLabel);

    gainAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::gain, gainSlider);
    cursorXAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorX, xSlider);
    cursorYAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorY, ySlider);
    cursorZAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorZ, zSlider);

    apvts.addParameterListener (ParameterIDs::cursorX, this);
    apvts.addParameterListener (ParameterIDs::cursorY, this);
    apvts.addParameterListener (ParameterIDs::cursorZ, this);
    updateReadoutAndGainsFromParams();

    trajectoryToggle.setButtonText ("Trajectory");
    trajectoryToggle.onClick = [this]
    {
        if (onTrajectoryChanged)
            onTrajectoryChanged (trajectoryToggle.getToggleState());
    };
    addAndMakeVisible (trajectoryToggle);

    cursorReadoutLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (cursorReadoutLabel);
}

CenterControlPanel::~CenterControlPanel()
{
    if (apvtsPtr != nullptr)
    {
        apvtsPtr->removeParameterListener (ParameterIDs::cursorX, this);
        apvtsPtr->removeParameterListener (ParameterIDs::cursorY, this);
        apvtsPtr->removeParameterListener (ParameterIDs::cursorZ, this);
    }
}

void CenterControlPanel::parameterChanged (const juce::String& parameterID, float /*newValue*/)
{
    if (parameterID == ParameterIDs::cursorX || parameterID == ParameterIDs::cursorY || parameterID == ParameterIDs::cursorZ)
        updateReadoutAndGainsFromParams();
}

void CenterControlPanel::updateReadoutAndGainsFromParams()
{
    if (apvtsPtr == nullptr)
        return;
    auto* px = apvtsPtr->getRawParameterValue (ParameterIDs::cursorX);
    auto* py = apvtsPtr->getRawParameterValue (ParameterIDs::cursorY);
    auto* pz = apvtsPtr->getRawParameterValue (ParameterIDs::cursorZ);
    const float x = (px != nullptr) ? px->load() : 0.5f;
    const float y = (py != nullptr) ? py->load() : 0.5f;
    const float z = (pz != nullptr) ? pz->load() : 0.5f;
    cursorReadoutLabel.setText ("Cursor: " + juce::String (x, 2) + ", " + juce::String (y, 2) + ", " + juce::String (z, 2),
                                juce::dontSendNotification);
}

void CenterControlPanel::setCursorChangedCallback (CursorChangedCallback callback)
{
    onCursorChanged = std::move (callback);
}

void CenterControlPanel::setTrajectoryChangedCallback (TrajectoryChangedCallback callback)
{
    onTrajectoryChanged = std::move (callback);
}

void CenterControlPanel::setCursorPosition (float x, float y, float z)
{
    xSlider.setValue (x, juce::dontSendNotification);
    ySlider.setValue (y, juce::dontSendNotification);
    zSlider.setValue (z, juce::dontSendNotification);
    updateCursorFromSliders();
}

void CenterControlPanel::setTrajectoryActive (bool isActive)
{
    trajectoryToggle.setToggleState (isActive, juce::dontSendNotification);
}

void CenterControlPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::grey.withAlpha (0.6f));
    g.drawRect (getLocalBounds(), 1);
}

void CenterControlPanel::resized()
{
    auto inner = getLocalBounds().reduced (8);
    auto controlsArea = inner.removeFromTop (juce::roundToInt (inner.getHeight() * 0.7f));
    constexpr int controlGap = 8;
    const auto controlWidth = (controlsArea.getWidth() - (controlGap * 3)) / 4;

    for (int control = 0; control < 4; ++control)
    {
        auto cell = controlsArea.removeFromLeft (controlWidth);
        if (control < 3)
            controlsArea.removeFromLeft (controlGap);

        auto sliderArea = cell.removeFromTop (juce::jmax (40, cell.getHeight() - 14));
        auto labelArea = cell;

        if (control == 0) { xSlider.setBounds (sliderArea); xLabel.setBounds (labelArea); }
        if (control == 1) { ySlider.setBounds (sliderArea); yLabel.setBounds (labelArea); }
        if (control == 2) { zSlider.setBounds (sliderArea); zLabel.setBounds (labelArea); }
        if (control == 3) { gainSlider.setBounds (sliderArea); gainLabel.setBounds (labelArea); }
    }

    inner.removeFromTop (4);
    auto trajectoryArea = inner.removeFromTop (24);
    trajectoryToggle.setBounds (trajectoryArea.removeFromLeft (120));
    cursorReadoutLabel.setBounds (trajectoryArea);
}

void CenterControlPanel::configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void CenterControlPanel::updateCursorFromSliders()
{
    const auto x = static_cast<float> (xSlider.getValue());
    const auto y = static_cast<float> (ySlider.getValue());
    const auto z = static_cast<float> (zSlider.getValue());

    cursorReadoutLabel.setText ("Cursor: " + juce::String (x, 2) + ", " + juce::String (y, 2) + ", " + juce::String (z, 2),
                                juce::dontSendNotification);
    if (onCursorChanged)
        onCursorChanged (x, y, z);
}
} // namespace UI
