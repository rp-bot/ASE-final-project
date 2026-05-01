#include "AmpEnvelopeSection.h"
#include "../Parameters/ParameterIDs.h"

namespace
{
    constexpr std::array<const char*, 6> kAmpNames {
        "Attack", "Decay", "Sustain", "Release", "Level", "Vel"
    };
}

namespace UI
{

AmpEnvelopeSection::AmpEnvelopeSection (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    setWantsKeyboardFocus (false);
    setMouseClickGrabsKeyboardFocus (false);

    ampControlTitle.setText ("Master Envelope Control", juce::dontSendNotification);
    ampControlTitle.setJustificationType (juce::Justification::topLeft);
    ampControlTitle.setColour (juce::Label::textColourId, accent.brighter (0.3f));
    addAndMakeVisible (ampControlTitle);

    envelopeEditor = std::make_unique<EnvelopeGraphEditor> (
        apvts,
        std::array<juce::String, 6> {
            ParameterIDs::masterAmp (0),
            ParameterIDs::masterAmp (1),
            ParameterIDs::masterAmp (2),
            ParameterIDs::masterAmp (3),
            ParameterIDs::masterAmp (4),
            ParameterIDs::masterAmp (5),
        },
        accent);
    envelopeEditor->setWantsKeyboardFocus (false);
    envelopeEditor->setMouseClickGrabsKeyboardFocus (false);
    addAndMakeVisible (*envelopeEditor);

    for (int i = 0; i < ampParams; ++i)
    {
        configureRotarySlider (ampSliders[static_cast<size_t> (i)],
                               ampLabels[static_cast<size_t> (i)],
                               kAmpNames[static_cast<size_t> (i)]);
        ampSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderOutlineColourId,
                                                       accent.withAlpha (0.25f));
        ampSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId,
                                                        accent.withAlpha (0.9f));
        ampLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId,
                                                      juce::Colours::whitesmoke.withAlpha (0.9f));
        addChildComponent (ampSliders[static_cast<size_t> (i)]);
        addChildComponent (ampLabels[static_cast<size_t> (i)]);

        ampAttachments[static_cast<size_t> (i)] =
            std::make_unique<SliderAttachment> (apvts,
                                                  ParameterIDs::masterAmp (i),
                                                  ampSliders[static_cast<size_t> (i)]);

        ampSliders[static_cast<size_t> (i)].onValueChange = [this, i] { propagateAmpMaster (i); };

        auto& valueEditor = ampValueEditors[static_cast<size_t> (i)];
        valueEditor.setMultiLine (false);
        valueEditor.setReturnKeyStartsNewLine (false);
        valueEditor.setJustification (juce::Justification::centred);
        valueEditor.setColour (juce::TextEditor::backgroundColourId, juce::Colours::black.withAlpha (0.2f));
        valueEditor.setColour (juce::TextEditor::outlineColourId, accent.withAlpha (0.5f));
        valueEditor.setColour (juce::TextEditor::textColourId, juce::Colours::whitesmoke);
        valueEditor.onReturnKey = [this, i] { commitAmpValueFromEditor (i); };
        valueEditor.onFocusLost = [this, i] { commitAmpValueFromEditor (i); };
        configureValueEditor (valueEditor, i);
        addAndMakeVisible (valueEditor);
    }

    const auto& kOscColours = masterSectionOscColours();
    for (int i = 0; i < oscComponents; ++i)
    {
        const auto oscColour = kOscColours[static_cast<size_t> (i)];
        auto& tb = ampToggles[static_cast<size_t> (i)];
        tb.setClickingTogglesState (true);
        tb.setWantsKeyboardFocus (false);
        tb.setColour (juce::TextButton::buttonColourId, oscColour.withAlpha (0.25f));
        tb.setColour (juce::TextButton::buttonOnColourId, oscColour.withAlpha (0.9f));
        tb.setColour (juce::ComboBox::outlineColourId, oscColour.withAlpha (0.75f));
        tb.setLookAndFeel (&toggleLookAndFeel);
        addAndMakeVisible (tb);
        ampToggleAttachments[static_cast<size_t> (i)] =
            std::make_unique<ButtonAttachment> (apvts, ParameterIDs::oscAmpEnabled (i), tb);

        tb.onClick = [this, i] {
            if (auto* focused = juce::Component::getCurrentlyFocusedComponent())
                focused->giveAwayKeyboardFocus();
            if (ampToggles[static_cast<size_t> (i)].getToggleState())
                seedCornerFromMaster (i);
        };
    }

    syncEditorsFromParameters();
    startTimerHz (20);
}

AmpEnvelopeSection::~AmpEnvelopeSection()
{
    for (auto& tb : ampToggles)
        tb.setLookAndFeel (nullptr);
    stopTimer();
}

void AmpEnvelopeSection::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void AmpEnvelopeSection::resized()
{
    auto bounds = getLocalBounds();

    const int toggleAreaWidth = 50;
    const int titleHeight = 24;
    const int valueEditorHeight = 24;
    const int padding = 1;
    const int toggleSize = toggleAreaWidth / 2;
    const int toggleGridH = toggleSize * 4;
    const int toggleSpacing = 2;

    auto ampTogglesArea = bounds.removeFromRight (toggleAreaWidth);

    ampControlTitle.setBounds (bounds.removeFromTop (titleHeight));

    auto ampValueRow = bounds.removeFromBottom (valueEditorHeight);
    if (envelopeEditor != nullptr)
        envelopeEditor->setBounds (bounds.reduced (padding));

    const int ampCellW = juce::jmax (1, ampValueRow.getWidth() / ampParams);
    for (int i = 0; i < ampParams; ++i)
    {
        const auto idx = static_cast<size_t> (i);
        auto cell = ampValueRow.removeFromLeft (ampCellW).reduced (1);
        ampValueEditors[idx].setBounds (cell);
    }

    const int ampStartY = ampTogglesArea.getY()
                          + titleHeight
                          + (ampTogglesArea.getHeight() - titleHeight - toggleGridH) / 2;
    for (int i = 0; i < oscComponents; ++i)
    {
        const int col = i / 4;
        const int row = i % 4;
        juce::Rectangle<int> cell (ampTogglesArea.getX() + col * toggleSize,
                                   ampStartY + row * toggleSize,
                                   toggleSize,
                                   toggleSize);
        ampToggles[static_cast<size_t> (i)].setBounds (cell.reduced (toggleSpacing / 2));
    }
}

void AmpEnvelopeSection::timerCallback()
{
    syncEditorsFromParameters();
}

void AmpEnvelopeSection::configureRotarySlider (juce::Slider& slider,
                                                 juce::Label& label,
                                                 const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void AmpEnvelopeSection::configureValueEditor (MasterDragValueEditor& editor, int index)
{
    if (apvtsPtr == nullptr)
        return;

    auto* parameter = apvtsPtr->getParameter (ParameterIDs::masterAmp (index));
    if (parameter == nullptr)
        return;

    const auto range = parameter->getNormalisableRange();
    editor.configure (
        range.start,
        range.end,
        [this, index]() {
            return static_cast<float> (ampSliders[static_cast<size_t> (index)].getValue());
        },
        [this, index, range](float value) {
            const auto clamped = juce::jlimit (range.start, range.end, value);
            if (auto* p = apvtsPtr->getParameter (ParameterIDs::masterAmp (index)))
                p->setValueNotifyingHost (range.convertTo0to1 (clamped));
        },
        false);
}

void AmpEnvelopeSection::syncEditorsFromParameters()
{
    for (int i = 0; i < ampParams; ++i)
    {
        const auto idx = static_cast<size_t> (i);
        auto& editor = ampValueEditors[idx];
        if (! editor.hasKeyboardFocus (true) || editor.isDraggingValue())
            editor.setText (formatMasterParameterDisplayValue (kAmpNames[idx],
                                                               static_cast<float> (ampSliders[idx].getValue())),
                            juce::dontSendNotification);
    }
}

void AmpEnvelopeSection::commitAmpValueFromEditor (int index)
{
    if (apvtsPtr == nullptr)
        return;
    const auto idx = static_cast<size_t> (index);
    const auto typed = parseMasterControlTextValue (ampValueEditors[idx].getText(), true);
    if (auto* p = apvtsPtr->getParameter (ParameterIDs::masterAmp (index)))
    {
        const auto range = p->getNormalisableRange();
        const auto clamped = juce::jlimit (range.start, range.end, typed);
        p->setValueNotifyingHost (range.convertTo0to1 (clamped));
    }
}

void AmpEnvelopeSection::propagateAmpMaster (int paramIndex)
{
    if (apvtsPtr == nullptr)
        return;

    const auto value = static_cast<float> (ampSliders[static_cast<size_t> (paramIndex)].getValue());
    for (int corner = 0; corner < oscComponents; ++corner)
    {
        if (ampToggles[static_cast<size_t> (corner)].getToggleState())
            writeParameterNormalised (*apvtsPtr, ParameterIDs::cornerAmpById (corner, paramIndex), value);
    }
}

void AmpEnvelopeSection::seedCornerFromMaster (int corner)
{
    if (apvtsPtr == nullptr)
        return;

    for (int i = 0; i < ampParams; ++i)
    {
        const auto value = static_cast<float> (ampSliders[static_cast<size_t> (i)].getValue());
        writeParameterNormalised (*apvtsPtr, ParameterIDs::cornerAmpById (corner, i), value);
    }
}

} // namespace UI
