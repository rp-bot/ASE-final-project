#include "CenterControlPanel.h"
#include "../Parameters/ParameterIDs.h"

namespace UI
{
CenterControlPanel::CenterControlPanel (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    // Knobs labeled X, Y, Z (left→right). Joystick: X + engine-Z axis; height + Z knob: engine Y (synced).
    configureCursorKnob (xSlider, xLabel, "X");
    configureCursorKnob (ySlider, yLabel, "Y");
    configureCursorKnob (zSlider, zLabel, "Z");
    configureHeightSlider (heightSlider, heightLabel, "Height");

    xSlider.onValueChange = [this] { updateCursorFromControls(); };
    ySlider.onValueChange = [this] { updateCursorFromControls(); };
    zSlider.onValueChange = [this]
    {
        if (syncingEngineY_)
            return;
        syncingEngineY_ = true;
        heightSlider.setValue (zSlider.getValue(), juce::sendNotificationSync);
        syncingEngineY_ = false;
        updateCursorFromControls();
    };
    heightSlider.onValueChange = [this]
    {
        if (syncingEngineY_)
            return;
        syncingEngineY_ = true;
        zSlider.setValue (heightSlider.getValue(), juce::dontSendNotification);
        syncingEngineY_ = false;
        updateCursorFromControls();
    };

    xyJoystick.setXYSliders (&xSlider, &ySlider);
    xyJoystick.setScrollSlider (&heightSlider);
    addAndMakeVisible (xyJoystick);

    addAndMakeVisible (xLabel);
    addAndMakeVisible (yLabel);
    addAndMakeVisible (zLabel);
    addAndMakeVisible (xSlider);
    addAndMakeVisible (ySlider);
    addAndMakeVisible (zSlider);
    addAndMakeVisible (heightLabel);
    addAndMakeVisible (heightSlider);

    cursorXAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorX, xSlider);
    cursorZAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorZ, ySlider);
    cursorYAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorY, heightSlider);

    apvts.addParameterListener (ParameterIDs::cursorX, this);
    apvts.addParameterListener (ParameterIDs::cursorY, this);
    apvts.addParameterListener (ParameterIDs::cursorZ, this);
    updateReadoutFromParams();
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
        updateReadoutFromParams();
}

void CenterControlPanel::syncEngineYControls (float normalizedY, bool fromHeightSlider)
{
    syncingEngineY_ = true;
    if (fromHeightSlider)
        zSlider.setValue (static_cast<double> (normalizedY), juce::dontSendNotification);
    else
        heightSlider.setValue (static_cast<double> (normalizedY), juce::dontSendNotification);
    syncingEngineY_ = false;
}

void CenterControlPanel::updateReadoutFromParams()
{
    if (apvtsPtr == nullptr)
        return;
    auto* py = apvtsPtr->getRawParameterValue (ParameterIDs::cursorY);
    const float y = (py != nullptr) ? py->load() : 0.5f;
    syncEngineYControls (y, false);
    xyJoystick.repaint();
}

void CenterControlPanel::setCursorChangedCallback (CursorChangedCallback callback)
{
    onCursorChanged = std::move (callback);
}

void CenterControlPanel::setCursorPosition (float x, float y, float z)
{
    xSlider.setValue (x, juce::dontSendNotification);
    ySlider.setValue (z, juce::dontSendNotification);
    syncingEngineY_ = true;
    heightSlider.setValue (y, juce::dontSendNotification);
    zSlider.setValue (y, juce::dontSendNotification);
    syncingEngineY_ = false;
    updateCursorFromControls();
    xyJoystick.repaint();
}

void CenterControlPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::grey.withAlpha (0.6f));
    g.drawRect (getLocalBounds(), 1);
}

void CenterControlPanel::resized()
{
    auto inner = getLocalBounds().reduced (8);
    auto bottomRow = inner.removeFromBottom (juce::jmax (100, juce::roundToInt (inner.getHeight() * 0.28f)));
    inner.removeFromBottom (6);

    constexpr int heightColumnWidth = 52;
    constexpr int gap = 8;
    auto heightColumn = inner.removeFromRight (heightColumnWidth);
    inner.removeFromRight (gap);

    auto topArea = inner;
    const int side = juce::jmin (topArea.getWidth(), topArea.getHeight());
    xyJoystick.setBounds (topArea.withSizeKeepingCentre (side, side));

    auto labelH = juce::jmin (18, heightColumn.getHeight() / 4);
    heightLabel.setBounds (heightColumn.removeFromTop (labelH));
    heightSlider.setBounds (heightColumn);

    constexpr int knobGap = 8;
    const int knobW = (bottomRow.getWidth() - knobGap * 2) / 3;
    constexpr int knobLabelH = 18;

    auto placeKnob = [knobLabelH] (juce::Rectangle<int> cell, juce::Label& label, juce::Slider& slider)
    {
        label.setBounds (cell.removeFromTop (knobLabelH));
        slider.setBounds (cell);
    };

    placeKnob (bottomRow.removeFromLeft (knobW), xLabel, xSlider);
    bottomRow.removeFromLeft (knobGap);
    placeKnob (bottomRow.removeFromLeft (knobW), yLabel, ySlider);
    bottomRow.removeFromLeft (knobGap);
    placeKnob (bottomRow, zLabel, zSlider);
}

void CenterControlPanel::configureCursorKnob (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setRange (0.0, 1.0, 0.001);
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 18);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::grey.withAlpha (0.6f));
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void CenterControlPanel::configureHeightSlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setRange (0.0, 1.0, 0.001);
    slider.setSliderStyle (juce::Slider::LinearVertical);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void CenterControlPanel::updateCursorFromControls()
{
    const float x = static_cast<float> (xSlider.getValue());
    const float engineY = static_cast<float> (heightSlider.getValue());
    const float engineZ = static_cast<float> (ySlider.getValue());

    if (onCursorChanged)
        onCursorChanged (x, engineY, engineZ);
}
} // namespace UI
