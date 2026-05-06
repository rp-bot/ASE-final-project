#include "CenterControlPanel.h"
#include "Parameters/ParameterIDs.h"
#include "UI/Common/SynthLookAndFeel.h"

namespace UI
{
CenterControlPanel::CenterControlPanel (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    configureSectionLabel (sceneSectionLabel_, "SCENE");
    configureSectionLabel (cursorSectionLabel_, "CURSOR");

    zoomLabel_.setText ("Zoom", juce::dontSendNotification);
    zoomLabel_.setJustificationType (juce::Justification::centredLeft);
    configureSceneSlider (zoomSlider_);
    zoomSlider_.setColour (juce::Slider::thumbColourId, SynthLookAndFeel::teal());
    zoomSlider_.setColour (juce::Slider::trackColourId, SynthLookAndFeel::arcTrack());
    zoomSlider_.setColour (juce::Slider::backgroundColourId, SynthLookAndFeel::arcTrack());

    configureCursorKnob (xKnob, "X");
    configureCursorKnob (yKnob, "Y");
    configureCursorKnob (zKnob, "Z");
    configureHeightSlider (heightSlider, heightLabel, "Height");
    configureSolidToggleButton (gizmoButton_);
    configureSolidToggleButton (zeroGButton_);

    xKnob.getSlider().onValueChange = [this] { updateCursorFromControls(); };
    yKnob.getSlider().onValueChange = [this]
    {
        if (syncingEngineY_)
            return;
        syncEngineYControls (static_cast<float> (yKnob.getSlider().getValue()), false);
        updateCursorFromControls();
    };
    zKnob.getSlider().onValueChange = [this] { updateCursorFromControls(); };
    heightSlider.onValueChange = [this]
    {
        if (syncingEngineY_)
            return;
        syncEngineYControls (static_cast<float> (heightSlider.getValue()), true);
        updateCursorFromControls();
    };

    xyJoystick.setXYSliders (&xKnob.getSlider(), &zKnob.getSlider());
    xyJoystick.setScrollSlider (&heightSlider);

    addAndMakeVisible (sceneSectionLabel_);
    addAndMakeVisible (zoomLabel_);
    addAndMakeVisible (zoomSlider_);
    addAndMakeVisible (gizmoButton_);

    addAndMakeVisible (cursorSectionLabel_);
    addAndMakeVisible (xyJoystick);
    addAndMakeVisible (xKnob);
    addAndMakeVisible (yKnob);
    addAndMakeVisible (zKnob);
    addAndMakeVisible (heightLabel);
    addAndMakeVisible (heightSlider);

    addAndMakeVisible (zeroGButton_);

    zoomAttachment_ = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cameraZoom, zoomSlider_);
    gizmoAttachment_ = std::make_unique<ButtonAttachment> (apvts, ParameterIDs::gizmoVisible, gizmoButton_);
    zeroGAttachment_ = std::make_unique<ButtonAttachment> (apvts, ParameterIDs::cubeZeroG, zeroGButton_);

    cursorXAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorX, xKnob.getSlider());
    cursorYAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorY, yKnob.getSlider());
    cursorZAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::cursorZ, zKnob.getSlider());

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
    {
        updateReadoutFromParams();
        return;
    }

}

void CenterControlPanel::syncEngineYControls (float normalizedY, bool fromHeightSlider)
{
    syncingEngineY_ = true;
    if (fromHeightSlider)
        yKnob.getSlider().setValue (static_cast<double> (normalizedY), juce::dontSendNotification);
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
    syncEngineYControls (y, true);
    xyJoystick.repaint();
}

void CenterControlPanel::setCursorChangedCallback (CursorChangedCallback callback)
{
    onCursorChanged = std::move (callback);
}

void CenterControlPanel::setCursorPosition (float x, float y, float z)
{
    xKnob.getSlider().setValue (x, juce::dontSendNotification);
    yKnob.getSlider().setValue (y, juce::dontSendNotification);
    zKnob.getSlider().setValue (z, juce::dontSendNotification);
    syncingEngineY_ = true;
    heightSlider.setValue (y, juce::dontSendNotification);
    syncingEngineY_ = false;
    updateCursorFromControls();
    xyJoystick.repaint();
}

void CenterControlPanel::paint (juce::Graphics& g)
{
    g.setColour (SynthLookAndFeel::panelBorder());
    g.drawRect (getLocalBounds(), 1);
    if (sceneDividerY_ > 0)
        g.drawLine (1.0f, static_cast<float> (sceneDividerY_), static_cast<float> (getWidth() - 1), static_cast<float> (sceneDividerY_));
    if (cursorDividerY_ > 0)
        g.drawLine (1.0f, static_cast<float> (cursorDividerY_), static_cast<float> (getWidth() - 1), static_cast<float> (cursorDividerY_));
}

void CenterControlPanel::resized()
{
    sceneDividerY_ = -1;
    cursorDividerY_ = -1;

    auto inner = getLocalBounds().reduced (6);
    constexpr int sectionHeaderH = 14;
    constexpr int sectionGap = 5;

    const int sceneSectionH = 76;

    auto sceneArea = inner.removeFromTop (juce::jmin (sceneSectionH, juce::jmax (30, inner.getHeight() / 4)));
    inner.removeFromTop (2);
    auto cursorArea = inner;

    sceneDividerY_ = sceneArea.getBottom() + 1;
    cursorDividerY_ = -1;

    {
        auto sceneHeader = sceneArea.removeFromTop (sectionHeaderH);
        sceneSectionLabel_.setBounds (sceneHeader);
        sceneArea.removeFromTop (sectionGap);

        auto topRow = sceneArea.removeFromTop (24).reduced (2, 0);
        auto gizmoArea = topRow.removeFromRight (86);
        topRow.removeFromRight (8);
        auto zeroGArea = topRow.removeFromRight (90);
        topRow.removeFromRight (8);
        auto zoomName = topRow.removeFromLeft (44);
        zoomLabel_.setBounds (zoomName);
        zoomSlider_.setBounds (topRow);
        zeroGButton_.setBounds (zeroGArea.withSizeKeepingCentre (juce::jmin (zeroGArea.getWidth(), 88), 22));
        gizmoButton_.setBounds (gizmoArea.withSizeKeepingCentre (juce::jmin (gizmoArea.getWidth(), 84), 22));

    }

    {
        auto cursorHeader = cursorArea.removeFromTop (sectionHeaderH);
        cursorSectionLabel_.setBounds (cursorHeader);
        cursorArea.removeFromTop (sectionGap);

        auto knobRow = cursorArea.removeFromBottom (juce::jmax (66, juce::roundToInt (cursorArea.getHeight() * 0.26f)));
        cursorArea.removeFromBottom (4);

        auto joystickArea = cursorArea;
        constexpr int heightColumnWidth = 52;
        joystickArea.removeFromRight (8);
        auto heightColumn = cursorArea.removeFromRight (heightColumnWidth);
        const int side = juce::jmin (joystickArea.getWidth(), joystickArea.getHeight());
        xyJoystick.setBounds (joystickArea.withSizeKeepingCentre (side, side));

        auto labelH = juce::jmin (18, heightColumn.getHeight() / 4);
        heightLabel.setBounds (heightColumn.removeFromTop (labelH));
        heightSlider.setBounds (heightColumn);

        const int knobGap = 8;
        const int knobW = (knobRow.getWidth() - knobGap * 2) / 3;
        auto placeCompactKnob = [] (juce::Rectangle<int> cell, LabelledKnob& knob)
        {
            const int side = juce::jmin (cell.getWidth(), cell.getHeight(), 70);
            knob.setBounds (cell.withSizeKeepingCentre (side, side));
        };

        placeCompactKnob (knobRow.removeFromLeft (knobW), xKnob);
        knobRow.removeFromLeft (knobGap);
        placeCompactKnob (knobRow.removeFromLeft (knobW), yKnob);
        knobRow.removeFromLeft (knobGap);
        placeCompactKnob (knobRow, zKnob);
    }

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
    slider.setColour (juce::Slider::textBoxOutlineColourId, SynthLookAndFeel::panelBorder());
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

void CenterControlPanel::configureSolidToggleButton (juce::Button& button)
{
    button.setClickingTogglesState (true);
    button.setColour (juce::TextButton::buttonColourId, SynthLookAndFeel::panelBorder().darker (0.30f));
    button.setColour (juce::TextButton::buttonOnColourId, SynthLookAndFeel::teal());
    button.setColour (juce::TextButton::textColourOffId, SynthLookAndFeel::textPrimary().withAlpha (0.95f));
    button.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
}

void CenterControlPanel::configureSectionLabel (juce::Label& label, const juce::String& text)
{
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centredLeft);
    label.setColour (juce::Label::textColourId, SynthLookAndFeel::textDim());
    label.setFont (juce::Font (11.0f, juce::Font::bold));
}

void CenterControlPanel::configureSceneSlider (juce::Slider& slider)
{
    slider.setRange (1.0, 20.0, 0.01);
    slider.setSliderStyle (juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
}

void CenterControlPanel::updateCursorFromControls()
{
    const float x = static_cast<float> (xKnob.getSlider().getValue());
    const float engineY = static_cast<float> (heightSlider.getValue());
    const float engineZ = static_cast<float> (zKnob.getSlider().getValue());

    if (onCursorChanged)
        onCursorChanged (x, engineY, engineZ);
}
} // namespace UI
