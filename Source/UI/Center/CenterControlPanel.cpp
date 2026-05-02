#include "CenterControlPanel.h"
#include "Parameters/ParameterIDs.h"

namespace UI
{
CenterControlPanel::CenterControlPanel (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    // Knobs labeled X, Y, Z (left→right). Joystick: X + engine-Z axis; height + Z knob: engine Y (synced).
    configureCursorKnob (xKnob, "X");
    configureCursorKnob (yKnob, "Y");
    configureCursorKnob (zKnob, "Z");
    configureHeightSlider (heightSlider, heightLabel, "Height");

    xKnob.getSlider().onValueChange = [this] { updateCursorFromControls(); };
    yKnob.getSlider().onValueChange = [this] { updateCursorFromControls(); };
    zKnob.getSlider().onValueChange = [this]
    {
        if (syncingEngineY_)
            return;
        syncingEngineY_ = true;
        heightSlider.setValue (zKnob.getSlider().getValue(), juce::sendNotificationSync);
        syncingEngineY_ = false;
        updateCursorFromControls();
    };
    heightSlider.onValueChange = [this]
    {
        if (syncingEngineY_)
            return;
        syncingEngineY_ = true;
        zKnob.getSlider().setValue (heightSlider.getValue(), juce::dontSendNotification);
        syncingEngineY_ = false;
        updateCursorFromControls();
    };

    xyJoystick.setXYSliders (&xKnob.getSlider(), &yKnob.getSlider());
    xyJoystick.setScrollSlider (&heightSlider);
    addAndMakeVisible (xyJoystick);

    addAndMakeVisible (xKnob);
    addAndMakeVisible (yKnob);
    addAndMakeVisible (zKnob);
    addAndMakeVisible (heightLabel);
    addAndMakeVisible (heightSlider);

    cursorXAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorX, xKnob.getSlider());
    cursorZAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorZ, yKnob.getSlider());
    cursorYAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorY, heightSlider);

    auto applyUnitDisplay = [] (LabelledKnob& knob)
    {
        auto& slider = knob.getSlider();
        slider.textFromValueFunction = [] (double v)
        {
            return juce::String (v, 3);
        };
        slider.valueFromTextFunction = [] (const juce::String& textIn)
        {
            return juce::jlimit (0.0, 1.0, textIn.trim().getDoubleValue());
        };
        knob.refreshValueText();
    };

    applyUnitDisplay (xKnob);
    applyUnitDisplay (yKnob);
    applyUnitDisplay (zKnob);

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
        zKnob.getSlider().setValue (static_cast<double> (normalizedY), juce::dontSendNotification);
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
    xKnob.getSlider().setValue (x, juce::dontSendNotification);
    yKnob.getSlider().setValue (z, juce::dontSendNotification);
    syncingEngineY_ = true;
    heightSlider.setValue (y, juce::dontSendNotification);
    zKnob.getSlider().setValue (y, juce::dontSendNotification);
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
    // Reduce inner padding and knob gap first as the panel shrinks.
    // Natural center panel width at default window size is ~372 px.
    const float ps = juce::jlimit (0.0f, 1.0f,
        (static_cast<float> (getWidth()) - 250.0f) / (372.0f - 250.0f));
    const int innerPad = 2 + juce::roundToInt (6.0f * ps); // 2..8, was fixed 8

    auto inner = getLocalBounds().reduced (innerPad);
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

    // Knob gap also shrinks with the panel so the three knobs keep more of their size.
    const int knobGap = 2 + juce::roundToInt (6.0f * ps); // 2..8, was fixed 8
    const int knobW = (bottomRow.getWidth() - knobGap * 2) / 3;

    auto placeKnob = [] (juce::Rectangle<int> cell, LabelledKnob& knob)
    {
        knob.setBounds (cell);
    };

    placeKnob (bottomRow.removeFromLeft (knobW), xKnob);
    bottomRow.removeFromLeft (knobGap);
    placeKnob (bottomRow.removeFromLeft (knobW), yKnob);
    bottomRow.removeFromLeft (knobGap);
    placeKnob (bottomRow, zKnob);
}

void CenterControlPanel::configureCursorKnob (LabelledKnob& knob, const juce::String& text)
{
    knob.setNameLabelText (text);
    knob.setValueFormatter (nullptr);
    knob.setValueEditExtraChars ("");

    auto& slider = knob.getSlider();
    slider.setRange (0.0, 1.0, 0.001);
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::grey.withAlpha (0.6f));
    slider.textFromValueFunction = [] (double v)
    {
        return juce::String (v, 3);
    };
    slider.valueFromTextFunction = [] (const juce::String& textIn)
    {
        return juce::jlimit (0.0, 1.0, textIn.trim().getDoubleValue());
    };
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
    const float x = static_cast<float> (xKnob.getSlider().getValue());
    const float engineY = static_cast<float> (heightSlider.getValue());
    const float engineZ = static_cast<float> (yKnob.getSlider().getValue());

    if (onCursorChanged)
        onCursorChanged (x, engineY, engineZ);
}
} // namespace UI
