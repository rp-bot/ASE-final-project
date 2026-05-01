#include "MasterControls.h"
#include "../Parameters/ParameterIDs.h"
#include <cmath>


namespace
{
    constexpr std::array<const char*, 4> kFilterNames
    {
        "Cutoff", "Res", "Key", "Drive"
    };

    constexpr std::array<const char*, 6> kAmpNames
    {
        "Attack", "Decay", "Sustain", "Release", "Level", "Vel"
    };

    // Per-oscillator accent palette. Must stay in sync with the moduleColours
    // array in VolumetricSynthEditor.cpp (corner index -> colour).
    const std::array<juce::Colour, 8> kOscColours
    {
        juce::Colour::fromRGB (231, 76, 60),   // red
        juce::Colour::fromRGB (230, 126, 34),  // orange
        juce::Colour::fromRGB (241, 196, 15),  // yellow
        juce::Colour::fromRGB (46, 204, 113),  // green
        juce::Colour::fromRGB (26, 188, 156),  // teal
        juce::Colour::fromRGB (52, 152, 219),  // blue
        juce::Colour::fromRGB (155, 89, 182),  // purple
        juce::Colour::fromRGB (236, 112, 173)  // pink
    };

    float parseTextValue (juce::String text, bool preferMilliseconds)
    {
        text = text.trim();
        if (text.endsWithIgnoreCase ("khz"))
            return static_cast<float> (text.dropLastCharacters (3).trimEnd().getDoubleValue() * 1000.0);
        if (text.endsWithIgnoreCase ("hz"))
            return static_cast<float> (text.dropLastCharacters (2).trimEnd().getDoubleValue());
        if (text.endsWithIgnoreCase ("ms"))
            return static_cast<float> (text.dropLastCharacters (2).trimEnd().getDoubleValue() / 1000.0);
        if (text.endsWithIgnoreCase ("s"))
            return static_cast<float> (text.dropLastCharacters (1).trimEnd().getDoubleValue());
        if (text.endsWith ("%"))
            return static_cast<float> (text.dropLastCharacters (1).trimEnd().getDoubleValue() / 100.0);
        if (text.endsWithIgnoreCase ("k"))
            return static_cast<float> (text.dropLastCharacters (1).getDoubleValue() * 1000.0);
        const auto raw = static_cast<float> (text.getDoubleValue());
        return preferMilliseconds ? raw / 1000.0f : raw;
    }
} //TODO: this could go into the header; copied from OscilatorModuleComponent.cpp for now

namespace UI {
    void MasterControls::DragValueEditor::configure (float minValue, float maxValue,
                                                     std::function<float()> valueGetter,
                                                     std::function<void(float)> valueSetter,
                                                     bool useLogDrag)
    {
        minV = minValue;
        maxV = maxValue;
        getter = std::move (valueGetter);
        setter = std::move (valueSetter);
        logDrag = useLogDrag;
    }

    void MasterControls::DragValueEditor::mouseDown (const juce::MouseEvent& event)
    {
        didDrag = false;
        dragStartY = event.getPosition().y;
        dragStartValue = getter != nullptr ? getter() : 0.0f;
        TextEditor::mouseDown (event);
    }

    void MasterControls::DragValueEditor::mouseDrag (const juce::MouseEvent& event)
    {
        if (setter == nullptr)
            return;

        const auto deltaY = static_cast<float> (dragStartY - event.getPosition().y);
        if (std::abs (deltaY) < 2.0f)
            return;

        didDrag = true;
        float newValue = dragStartValue;
        if (logDrag && minV > 0.0f && maxV > minV)
        {
            const auto ratio = maxV / minV;
            const auto startNorm = std::log (juce::jmax (dragStartValue, minV) / minV) / std::log (ratio);
            const auto normDelta = deltaY / 180.0f;
            const auto nextNorm = juce::jlimit (0.0f, 1.0f, startNorm + normDelta);
            newValue = minV * std::pow (ratio, nextNorm);
        }
        else
        {
            const auto sensitivity = juce::jmax (0.0001f, (maxV - minV) / 160.0f);
            newValue = juce::jlimit (minV, maxV, dragStartValue + deltaY * sensitivity);
        }
        setter (newValue);
    }

    void MasterControls::DragValueEditor::mouseUp (const juce::MouseEvent& event)
    {
        if (! didDrag)
            TextEditor::mouseUp (event);
        didDrag = false;
    }

    MasterControls::MasterControls(juce::AudioProcessorValueTreeState &apvts) : apvtsPtr (&apvts) {
        setWantsKeyboardFocus (false);
        setMouseClickGrabsKeyboardFocus (false);

        // DRAW FILTER MASTER CONTROL
        // Text
        filterControlTitle.setText("Master Filter Control", juce::dontSendNotification);
        filterControlTitle.setJustificationType(juce::Justification::topLeft);
        filterControlTitle.setColour(juce::Label::textColourId, accent.brighter(0.3f));
        addAndMakeVisible(filterControlTitle);

        filterEditor = std::make_unique<FilterResponseEditor> (
            apvts,
            std::array<juce::String, 4> {
                ParameterIDs::masterFilter (0),
                ParameterIDs::masterFilter (1),
                ParameterIDs::masterFilter (2),
                ParameterIDs::masterFilter (3)
            },
            accent);
        filterEditor->setWantsKeyboardFocus (false);
        filterEditor->setMouseClickGrabsKeyboardFocus (false);
        addAndMakeVisible (*filterEditor);

        // Knobs
        for (int i = 0; i < filterParams; ++i)
        {
            configureRotarySlider (filterSliders[static_cast<size_t> (i)], filterLabels[static_cast<size_t> (i)],
                                   kFilterNames[static_cast<size_t> (i)]);
            filterSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderOutlineColourId,
                                                              accent.withAlpha (0.25f));
            filterSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId,
                                                              accent.withAlpha (0.9f));
            filterLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId,
                                                             juce::Colours::whitesmoke.withAlpha (0.9f));
            addChildComponent (filterSliders[static_cast<size_t> (i)]);
            addChildComponent (filterLabels[static_cast<size_t> (i)]);

            // SliderAttachment automatically applies the parameter's range, skew and
            // default to the slider, so no manual setRange/setSkewFactor needed.
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
            configureValueEditor (valueEditor, true, i);
            addAndMakeVisible (valueEditor);
        }

        // Toggle Buttons (TextButton with toggle behaviour -> coloured rectangles)
        for (int i = 0; i < oscComponents; ++i)
        {
            const auto oscColour = kOscColours[static_cast<size_t> (i)];
            auto& tb = filterToggles[static_cast<size_t> (i)];
            tb.setClickingTogglesState (true);
            tb.setWantsKeyboardFocus (false);
            tb.setColour (juce::TextButton::buttonColourId,   oscColour.withAlpha (0.25f));
            tb.setColour (juce::TextButton::buttonOnColourId, oscColour.withAlpha (0.9f));
            tb.setColour (juce::ComboBox::outlineColourId,    oscColour.withAlpha (0.75f));
            tb.setLookAndFeel (&toggleLookAndFeel);
            addAndMakeVisible (tb);
            filterToggleAttachments[static_cast<size_t> (i)] = std::make_unique<ButtonAttachment> (apvts, ParameterIDs::oscFilterEnabled (i), tb);

            // onClick fires after ButtonAttachment has updated state, so reading
            // getToggleState here reflects the new value (push on off->on only).
            tb.onClick = [this, i] {
                if (auto* focused = juce::Component::getCurrentlyFocusedComponent())
                    focused->giveAwayKeyboardFocus();
                if (filterToggles[static_cast<size_t> (i)].getToggleState())
                    seedCornerFromMaster (i, true);
            };
        }

        // DRAW ENVELOPE MASTER CONTROL
        // Text
        ampControlTitle.setText("Master Envelope Control", juce::dontSendNotification);
        ampControlTitle.setJustificationType(juce::Justification::topLeft);
        ampControlTitle.setColour(juce::Label::textColourId, accent.brighter(0.3f));
        addAndMakeVisible(ampControlTitle);

        envelopeEditor = std::make_unique<EnvelopeGraphEditor> (
            apvts,
            std::array<juce::String, 6> {
                ParameterIDs::masterAmp (0),
                ParameterIDs::masterAmp (1),
                ParameterIDs::masterAmp (2),
                ParameterIDs::masterAmp (3),
                ParameterIDs::masterAmp (4),
                ParameterIDs::masterAmp (5)
            },
            accent);
        envelopeEditor->setWantsKeyboardFocus (false);
        envelopeEditor->setMouseClickGrabsKeyboardFocus (false);
        addAndMakeVisible (*envelopeEditor);

        // Knobs
        for (int i = 0; i < ampParams; ++i)
        {
            configureRotarySlider (ampSliders[static_cast<size_t> (i)], ampLabels[static_cast<size_t> (i)],
                                   kAmpNames[static_cast<size_t> (i)]);
            ampSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderOutlineColourId,
                                                          accent.withAlpha (0.25f));
            ampSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId,
                                                           accent.withAlpha (0.9f));
            ampLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId,
                                                          juce::Colours::whitesmoke.withAlpha (0.9f));
            addChildComponent (ampSliders[static_cast<size_t> (i)]);
            addChildComponent (ampLabels[static_cast<size_t> (i)]);

            ampAttachments[static_cast<size_t> (i)] = std::make_unique<SliderAttachment> (apvts,
                                       ParameterIDs::masterAmp (i), ampSliders[static_cast<size_t> (i)]);

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
            configureValueEditor (valueEditor, false, i);
            addAndMakeVisible (valueEditor);
        }

        // Toggle Buttons (TextButton with toggle behaviour -> coloured rectangles)
        for (int i = 0; i < oscComponents; ++i)
        {
            const auto oscColour = kOscColours[static_cast<size_t> (i)];
            auto& tb = ampToggles[static_cast<size_t> (i)];
            tb.setClickingTogglesState (true);
            tb.setWantsKeyboardFocus (false);
            tb.setColour (juce::TextButton::buttonColourId,   oscColour.withAlpha (0.25f));
            tb.setColour (juce::TextButton::buttonOnColourId, oscColour.withAlpha (0.9f));
            tb.setColour (juce::ComboBox::outlineColourId,    oscColour.withAlpha (0.75f));
            tb.setLookAndFeel (&toggleLookAndFeel);
            addAndMakeVisible (tb);
            ampToggleAttachments[static_cast<size_t> (i)] =
                std::make_unique<ButtonAttachment> (apvts, ParameterIDs::oscAmpEnabled (i), tb);

            tb.onClick = [this, i]
            {
                if (auto* focused = juce::Component::getCurrentlyFocusedComponent())
                    focused->giveAwayKeyboardFocus();
                if (ampToggles[static_cast<size_t> (i)].getToggleState())
                    seedCornerFromMaster (i, false);
            };
        }

        syncEditorsFromParameters();
        startTimerHz (20);
    }

    MasterControls::~MasterControls()
    {
        for (auto& tb : filterToggles) tb.setLookAndFeel (nullptr);
        for (auto& tb : ampToggles)    tb.setLookAndFeel (nullptr);
        stopTimer();
    }

    void MasterControls::resized()
    {
        auto bounds = getLocalBounds();

        // Split vertically into two halves
        auto filterSection = bounds.removeFromTop (bounds.getHeight() / 2);
        auto ampSection    = bounds; // remainder

        const int toggleAreaWidth = 50;
        const int titleHeight     = 24;
        const int valueEditorHeight = 24;
        const int padding         = 1;
        const int toggleSize      = toggleAreaWidth / 2;   // 25x25 cell per button (2 cols * 4 rows)
        const int toggleGridH     = toggleSize * 4;         // total height occupied by one 4-row group
        const int toggleSpacing   = 2;                      // visual gap between adjacent toggle squares

        // FILTER SECTION
        auto filterTogglesArea = filterSection.removeFromRight (toggleAreaWidth);

        // Title + visual editor (knobs are hidden to prioritize graph area)
        filterControlTitle.setBounds(filterSection.removeFromTop(titleHeight));

        auto filterValueRow = filterSection.removeFromBottom (valueEditorHeight);
        if (filterEditor != nullptr)
            filterEditor->setBounds (filterSection.reduced (padding));
        const int filterCellW = juce::jmax (1, filterValueRow.getWidth() / filterParams);
        for (int i = 0; i < filterParams; ++i)
        {
            const auto idx = static_cast<size_t> (i);
            auto cell = filterValueRow.removeFromLeft (filterCellW).reduced (1);
            filterValueEditors[idx].setBounds (cell);
        }

        // Toggle buttons (2 columns of 4), centred in available section below title
        const int filterStartY = filterTogglesArea.getY()
                              + titleHeight
                              + (filterTogglesArea.getHeight() - titleHeight - toggleGridH) / 2;
        for (int i = 0; i < oscComponents; ++i)
        {
            int col = i / 4; //TODO: hard coded in rn
            int row = i % 4;
            juce::Rectangle<int> cell (filterTogglesArea.getX() + col * toggleSize,
                                       filterStartY + row * toggleSize,
                                       toggleSize, toggleSize);
            filterToggles[static_cast<size_t> (i)].setBounds (cell.reduced (toggleSpacing / 2));
        }

        // AMP SECTION
        auto ampTogglesArea = ampSection.removeFromRight (toggleAreaWidth);

        ampControlTitle.setBounds(ampSection.removeFromTop(titleHeight));

        auto ampValueRow = ampSection.removeFromBottom (valueEditorHeight);
        if (envelopeEditor != nullptr)
            envelopeEditor->setBounds (ampSection.reduced (padding));
        const int ampCellW = juce::jmax (1, ampValueRow.getWidth() / ampParams);
        for (int i = 0; i < ampParams; ++i)
        {
            const auto idx = static_cast<size_t> (i);
            auto cell = ampValueRow.removeFromLeft (ampCellW).reduced (1);
            ampValueEditors[idx].setBounds (cell);
        }

        // Toggle buttons (2 columns of 4), centred in available section below title
        const int ampStartY = ampTogglesArea.getY()
                            + titleHeight
                            + (ampTogglesArea.getHeight() - titleHeight - toggleGridH) / 2;
        for (int i = 0; i < oscComponents; ++i)
        {
            int col = i / 4; //TODO: hard coded in rn
            int row = i % 4;
            juce::Rectangle<int> cell (ampTogglesArea.getX() + col * toggleSize,
                                       ampStartY + row * toggleSize,
                                       toggleSize, toggleSize);
            ampToggles[static_cast<size_t> (i)].setBounds (cell.reduced (toggleSpacing / 2));
        }
    }

    // COPIED FROM OscillatorModuleComponent.cpp
    void MasterControls::configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        label.setText (text, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
    }


    void MasterControls::paint(juce::Graphics& g)
    {
        juce::ignoreUnused (g);
    }

    void MasterControls::timerCallback()
    {
        syncEditorsFromParameters();
    }

    void MasterControls::setSpectrumDataSource (const std::array<std::atomic<float>, 128>* bins,
                                                const std::atomic<float>* sampleRateHz)
    {
        if (filterEditor != nullptr)
            filterEditor->setSpectrumData (bins, sampleRateHz);
    }

    void MasterControls::configureValueEditor (DragValueEditor& editor, bool isFilter, int index)
    {
        if (apvtsPtr == nullptr)
            return;

        const auto parameterId = isFilter ? ParameterIDs::masterFilter (index) : ParameterIDs::masterAmp (index);
        auto* parameter = apvtsPtr->getParameter (parameterId);
        if (parameter == nullptr)
            return;

        const auto range = parameter->getNormalisableRange();
        editor.configure (
            range.start,
            range.end,
            [this, isFilter, index]()
            {
                return static_cast<float> (isFilter
                    ? filterSliders[static_cast<size_t> (index)].getValue()
                    : ampSliders[static_cast<size_t> (index)].getValue());
            },
            [this, isFilter, index, range](float value)
            {
                const auto clamped = juce::jlimit (range.start, range.end, value);
                if (auto* p = apvtsPtr->getParameter (isFilter ? ParameterIDs::masterFilter (index)
                                                               : ParameterIDs::masterAmp (index)))
                    p->setValueNotifyingHost (range.convertTo0to1 (clamped));
            },
            isFilter && index == 0);
    }

    void MasterControls::syncEditorsFromParameters()
    {
        for (int i = 0; i < filterParams; ++i)
        {
            const auto idx = static_cast<size_t> (i);
            auto& editor = filterValueEditors[idx];
            if (! editor.hasKeyboardFocus (true) || editor.isDraggingValue())
                editor.setText (formatParameterValue (kFilterNames[idx],
                                                      static_cast<float> (filterSliders[idx].getValue())),
                                juce::dontSendNotification);
        }

        for (int i = 0; i < ampParams; ++i)
        {
            const auto idx = static_cast<size_t> (i);
            auto& editor = ampValueEditors[idx];
            if (! editor.hasKeyboardFocus (true) || editor.isDraggingValue())
                editor.setText (formatParameterValue (kAmpNames[idx],
                                                      static_cast<float> (ampSliders[idx].getValue())),
                                juce::dontSendNotification);
        }
    }

    void MasterControls::commitFilterValueFromEditor (int index)
    {
        if (apvtsPtr == nullptr)
            return;
        const auto idx = static_cast<size_t> (index);
        const auto typed = parseTextValue (filterValueEditors[idx].getText(), false);
        if (auto* p = apvtsPtr->getParameter (ParameterIDs::masterFilter (index)))
        {
            const auto range = p->getNormalisableRange();
            const auto clamped = juce::jlimit (range.start, range.end, typed);
            p->setValueNotifyingHost (range.convertTo0to1 (clamped));
        }
    }

    void MasterControls::commitAmpValueFromEditor (int index)
    {
        if (apvtsPtr == nullptr)
            return;
        const auto idx = static_cast<size_t> (index);
        const auto typed = parseTextValue (ampValueEditors[idx].getText(), true);
        if (auto* p = apvtsPtr->getParameter (ParameterIDs::masterAmp (index)))
        {
            const auto range = p->getNormalisableRange();
            const auto clamped = juce::jlimit (range.start, range.end, typed);
            p->setValueNotifyingHost (range.convertTo0to1 (clamped));
        }
    }

    juce::String MasterControls::formatParameterValue (const juce::String& name, float value)
    {
        if (name == "Cutoff")
            return value >= 1000.0f ? juce::String (value / 1000.0f, 2) + " kHz" : juce::String (value, 0) + " Hz";
        if (name == "Res" || name == "Key" || name == "Drive")
            return juce::String (juce::roundToInt (value * 100.0f)) + " %";
        if (name == "Attack" || name == "Decay" || name == "Release")
        {
            if (value < 1.0f)
                return juce::String (juce::roundToInt (value * 1000.0f)) + " ms";
            return juce::String (value, 2) + " s";
        }
        return juce::String (value, 2);
    }

    void MasterControls::propagateFilterMaster (int paramIndex)
    {
        if (apvtsPtr == nullptr)
            return;

        const auto value = static_cast<float> (filterSliders[static_cast<size_t> (paramIndex)].getValue());
        for (int corner = 0; corner < oscComponents; ++corner)
        {
            if (filterToggles[static_cast<size_t> (corner)].getToggleState())
                writeNormalised (*apvtsPtr, ParameterIDs::cornerFilterById (corner, paramIndex), value);
        }
    }

    void MasterControls::propagateAmpMaster (int paramIndex)
    {
        if (apvtsPtr == nullptr) return;

        const auto value = static_cast<float> (ampSliders[static_cast<size_t> (paramIndex)].getValue());
        for (int corner = 0; corner < oscComponents; ++corner)
        {
            if (ampToggles[static_cast<size_t> (corner)].getToggleState())
                writeNormalised (*apvtsPtr, ParameterIDs::cornerAmpById (corner, paramIndex), value);
        }
    }

    void MasterControls::seedCornerFromMaster (int corner, bool isFilter)
    {
        if (apvtsPtr == nullptr) return;

        if (isFilter) {
            for (int i = 0; i < filterParams; ++i) {
                const auto value = static_cast<float> (filterSliders[static_cast<size_t> (i)].getValue());
                writeNormalised (*apvtsPtr, ParameterIDs::cornerFilterById (corner, i), value);
            }
        }
        else {
            for (int i = 0; i < ampParams; ++i) {
                const auto value = static_cast<float> (ampSliders[static_cast<size_t> (i)].getValue());
                writeNormalised (*apvtsPtr, ParameterIDs::cornerAmpById (corner, i), value);
            }
        }
    }

    void MasterControls::writeNormalised (juce::AudioProcessorValueTreeState& apvts,
                                          const juce::String& parameterId, float realValue) {
        if (auto* p = apvts.getParameter (parameterId))
            p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 (realValue));
    }




}
