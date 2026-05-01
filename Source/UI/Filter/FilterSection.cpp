#include "FilterSection.h"
#include "Parameters/ParameterIDs.h"

namespace
{
    constexpr std::array<const char*, 4> kFilterNames {
        "Cutoff", "Res", "Key", "Drive"
    };
}

namespace UI
{

FilterSection::FilterSection (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    setWantsKeyboardFocus (false);
    setMouseClickGrabsKeyboardFocus (false);

    filterControlTitle.setText ("Master Filter Control", juce::dontSendNotification);
    filterControlTitle.setJustificationType (juce::Justification::topLeft);
    filterControlTitle.setColour (juce::Label::textColourId, accent.brighter (0.3f));
    addAndMakeVisible (filterControlTitle);

    filterEditor = std::make_unique<FilterResponseEditor> (
        apvts,
        std::array<juce::String, 4> {
            ParameterIDs::masterFilter (0),
            ParameterIDs::masterFilter (1),
            ParameterIDs::masterFilter (2),
            ParameterIDs::masterFilter (3),
        },
        accent);
    filterEditor->setWantsKeyboardFocus (false);
    filterEditor->setMouseClickGrabsKeyboardFocus (false);
    addAndMakeVisible (*filterEditor);

    for (int i = 0; i < filterParams; ++i)
    {
        configureRotarySlider (filterSliders[static_cast<size_t> (i)],
                               filterLabels[static_cast<size_t> (i)],
                               kFilterNames[static_cast<size_t> (i)]);
        filterSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderOutlineColourId,
                                                          accent.withAlpha (0.25f));
        filterSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId,
                                                           accent.withAlpha (0.9f));
        filterLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId,
                                                          juce::Colours::whitesmoke.withAlpha (0.9f));
        addChildComponent (filterSliders[static_cast<size_t> (i)]);
        addChildComponent (filterLabels[static_cast<size_t> (i)]);

        filterParamsAttachments[static_cast<size_t> (i)] =
            std::make_unique<SliderAttachment> (apvts,
                                                  ParameterIDs::masterFilter (i),
                                                  filterSliders[static_cast<size_t> (i)]);

        filterSliders[static_cast<size_t> (i)].onValueChange = [this, i] { propagateFilterMaster (i); };

        auto& valueEditor = filterValueEditors[static_cast<size_t> (i)];
        valueEditor.setMultiLine (false);
        valueEditor.setReturnKeyStartsNewLine (false);
        valueEditor.setJustification (juce::Justification::centred);
        valueEditor.setColour (juce::TextEditor::backgroundColourId, juce::Colours::black.withAlpha (0.2f));
        valueEditor.setColour (juce::TextEditor::outlineColourId, accent.withAlpha (0.5f));
        valueEditor.setColour (juce::TextEditor::textColourId, juce::Colours::whitesmoke);
        valueEditor.onReturnKey = [this, i] { commitFilterValueFromEditor (i); };
        valueEditor.onFocusLost = [this, i] { commitFilterValueFromEditor (i); };
        configureValueEditor (valueEditor, i);
        addAndMakeVisible (valueEditor);
    }

    const auto& kOscColours = masterSectionOscColours();
    for (int i = 0; i < oscComponents; ++i)
    {
        const auto oscColour = kOscColours[static_cast<size_t> (i)];
        auto& tb = filterToggles[static_cast<size_t> (i)];
        tb.setClickingTogglesState (true);
        tb.setWantsKeyboardFocus (false);
        tb.setColour (juce::TextButton::buttonColourId, oscColour.withAlpha (0.25f));
        tb.setColour (juce::TextButton::buttonOnColourId, oscColour.withAlpha (0.9f));
        tb.setColour (juce::ComboBox::outlineColourId, oscColour.withAlpha (0.75f));
        tb.setLookAndFeel (&toggleLookAndFeel);
        addAndMakeVisible (tb);
        filterToggleAttachments[static_cast<size_t> (i)] =
            std::make_unique<ButtonAttachment> (apvts, ParameterIDs::oscFilterEnabled (i), tb);

        tb.onClick = [this, i] {
            if (auto* focused = juce::Component::getCurrentlyFocusedComponent())
                focused->giveAwayKeyboardFocus();
            if (filterToggles[static_cast<size_t> (i)].getToggleState())
                seedCornerFromMaster (i);
        };
    }

    syncEditorsFromParameters();
    startTimerHz (20);
}

FilterSection::~FilterSection()
{
    for (auto& tb : filterToggles)
        tb.setLookAndFeel (nullptr);
    stopTimer();
}

void FilterSection::paint (juce::Graphics& g)
{
    juce::ignoreUnused (g);
}

void FilterSection::resized()
{
    auto bounds = getLocalBounds();

    const int toggleAreaWidth = 50;
    const int titleHeight = 24;
    const int valueEditorHeight = 24;
    const int padding = 1;
    const int toggleSize = toggleAreaWidth / 2;
    const int toggleGridH = toggleSize * 4;
    const int toggleSpacing = 2;

    auto filterTogglesArea = bounds.removeFromRight (toggleAreaWidth);

    filterControlTitle.setBounds (bounds.removeFromTop (titleHeight));

    auto filterValueRow = bounds.removeFromBottom (valueEditorHeight);
    if (filterEditor != nullptr)
        filterEditor->setBounds (bounds.reduced (padding));

    const int filterCellW = juce::jmax (1, filterValueRow.getWidth() / filterParams);
    for (int i = 0; i < filterParams; ++i)
    {
        const auto idx = static_cast<size_t> (i);
        auto cell = filterValueRow.removeFromLeft (filterCellW).reduced (1);
        filterValueEditors[idx].setBounds (cell);
    }

    const int filterStartY = filterTogglesArea.getY()
                             + titleHeight
                             + (filterTogglesArea.getHeight() - titleHeight - toggleGridH) / 2;
    for (int i = 0; i < oscComponents; ++i)
    {
        const int col = i / 4;
        const int row = i % 4;
        juce::Rectangle<int> cell (filterTogglesArea.getX() + col * toggleSize,
                                   filterStartY + row * toggleSize,
                                   toggleSize,
                                   toggleSize);
        filterToggles[static_cast<size_t> (i)].setBounds (cell.reduced (toggleSpacing / 2));
    }
}

void FilterSection::timerCallback()
{
    syncEditorsFromParameters();
}

void FilterSection::setSpectrumDataSource (const std::array<std::atomic<float>, 128>* bins,
                                           const std::atomic<float>* sampleRateHz)
{
    if (filterEditor != nullptr)
        filterEditor->setSpectrumData (bins, sampleRateHz);
}

void FilterSection::configureRotarySlider (juce::Slider& slider,
                                           juce::Label& label,
                                           const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void FilterSection::configureValueEditor (MasterDragValueEditor& editor, int index)
{
    if (apvtsPtr == nullptr)
        return;

    auto* parameter = apvtsPtr->getParameter (ParameterIDs::masterFilter (index));
    if (parameter == nullptr)
        return;

    const auto range = parameter->getNormalisableRange();
    editor.configure (
        range.start,
        range.end,
        [this, index]() {
            return static_cast<float> (filterSliders[static_cast<size_t> (index)].getValue());
        },
        [this, index, range](float value) {
            const auto clamped = juce::jlimit (range.start, range.end, value);
            if (auto* p = apvtsPtr->getParameter (ParameterIDs::masterFilter (index)))
                p->setValueNotifyingHost (range.convertTo0to1 (clamped));
        },
        index == 0);
}

void FilterSection::syncEditorsFromParameters()
{
    for (int i = 0; i < filterParams; ++i)
    {
        const auto idx = static_cast<size_t> (i);
        auto& editor = filterValueEditors[idx];
        if (! editor.hasKeyboardFocus (true) || editor.isDraggingValue())
            editor.setText (formatMasterParameterDisplayValue (kFilterNames[idx],
                                                               static_cast<float> (filterSliders[idx].getValue())),
                            juce::dontSendNotification);
    }
}

void FilterSection::commitFilterValueFromEditor (int index)
{
    if (apvtsPtr == nullptr)
        return;
    const auto idx = static_cast<size_t> (index);
    const auto typed = parseMasterControlTextValue (filterValueEditors[idx].getText(), false);
    if (auto* p = apvtsPtr->getParameter (ParameterIDs::masterFilter (index)))
    {
        const auto range = p->getNormalisableRange();
        const auto clamped = juce::jlimit (range.start, range.end, typed);
        p->setValueNotifyingHost (range.convertTo0to1 (clamped));
    }
}

void FilterSection::propagateFilterMaster (int paramIndex)
{
    if (apvtsPtr == nullptr)
        return;

    const auto value = static_cast<float> (filterSliders[static_cast<size_t> (paramIndex)].getValue());
    for (int corner = 0; corner < oscComponents; ++corner)
    {
        if (filterToggles[static_cast<size_t> (corner)].getToggleState())
            writeParameterNormalised (*apvtsPtr, ParameterIDs::cornerFilterById (corner, paramIndex), value);
    }
}

void FilterSection::seedCornerFromMaster (int corner)
{
    if (apvtsPtr == nullptr)
        return;

    for (int i = 0; i < filterParams; ++i)
    {
        const auto value = static_cast<float> (filterSliders[static_cast<size_t> (i)].getValue());
        writeParameterNormalised (*apvtsPtr, ParameterIDs::cornerFilterById (corner, i), value);
    }
}

} // namespace UI
