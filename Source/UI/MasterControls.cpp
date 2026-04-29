#pragma once
#include "MasterControls.h"
#include "OscillatorModuleComponent.h"
#include "../Parameters/ParameterIDs.h"


namespace
{
    constexpr std::array<int, 5> kSynthKnobIndices { 0, 1, 3, 4, 5 };

    constexpr std::array<const char*, UI::OscillatorModuleComponent::filterParams> kFilterNames
    {
        "Cutoff", "Res", "Key", "Drive"
    };

    constexpr std::array<const char*, UI::OscillatorModuleComponent::ampParams> kAmpNames
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
} //TODO: this could go into the header; copied from OscilatorModuleComponent.cpp for now

namespace UI {
    MasterControls::MasterControls(juce::AudioProcessorValueTreeState &apvts) : apvtsPtr (&apvts) {

        // DRAW FILTER MASTER CONTROL
        // Text
        filterControlTitle.setText("Master Filter Control", juce::dontSendNotification);
        filterControlTitle.setJustificationType(juce::Justification::topLeft);
        filterControlTitle.setColour(juce::Label::textColourId, accent.brighter(0.3f));
        addAndMakeVisible(filterControlTitle);

        // Visualizer Box

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
            addAndMakeVisible (filterSliders[static_cast<size_t> (i)]);
            addAndMakeVisible (filterLabels[static_cast<size_t> (i)]);

            // SliderAttachment automatically applies the parameter's range, skew and
            // default to the slider, so no manual setRange/setSkewFactor needed.
            filterParamsAttachments[static_cast<size_t> (i)] =
                std::make_unique<SliderAttachment> (apvts,
                                                    ParameterIDs::masterFilter (i),
                                                    filterSliders[static_cast<size_t> (i)]);

            filterSliders[static_cast<size_t> (i)].onValueChange = [this, i] { propagateFilterMaster (i); };
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

        // Visualizer Box

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
            addAndMakeVisible (ampSliders[static_cast<size_t> (i)]);
            addAndMakeVisible (ampLabels[static_cast<size_t> (i)]);

            ampAttachments[static_cast<size_t> (i)] = std::make_unique<SliderAttachment> (apvts,
                                       ParameterIDs::masterAmp (i), ampSliders[static_cast<size_t> (i)]);

            ampSliders[static_cast<size_t> (i)].onValueChange = [this, i] { propagateAmpMaster (i); };
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
                if (ampToggles[static_cast<size_t> (i)].getToggleState())
                    seedCornerFromMaster (i, false);
            };
        }

    }

    MasterControls::~MasterControls()
    {
        for (auto& tb : filterToggles) tb.setLookAndFeel (nullptr);
        for (auto& tb : ampToggles)    tb.setLookAndFeel (nullptr);
    }

    void MasterControls::resized()
    {
        auto bounds = getLocalBounds();

        // Split vertically into two halves
        auto filterSection = bounds.removeFromTop (bounds.getHeight() / 2);
        auto ampSection    = bounds; // remainder

        const int toggleAreaWidth = 50;
        const int titleHeight     = 40;
        const int knobAreaHeight  = 100;
        const int knobLabelHeight = 20;                     // bottom strip of each knob cell reserved for the text label
        const int padding         = 1;
        const int toggleSize      = toggleAreaWidth / 2;   // 25x25 cell per button (2 cols * 4 rows)
        const int toggleGridH     = toggleSize * 4;         // total height occupied by one 4-row group
        const int toggleSpacing   = 2;                      // visual gap between adjacent toggle squares

        // FILTER SECTOIN
        auto filterTogglesArea = filterSection.removeFromRight (toggleAreaWidth);

        // Title, Knobs (seperate rows)
        filterControlTitle.setBounds(filterSection.removeFromTop(titleHeight));
        auto filterTopRow = filterSection.removeFromTop(knobAreaHeight);
        int knobW = filterTopRow.getWidth() / filterParams;
        for (int i = 0; i < filterParams; ++i)
        {
            auto knobArea = filterTopRow.removeFromLeft(knobW);
            filterLabels[i].setBounds(knobArea.removeFromBottom(knobLabelHeight));
            filterSliders[i].setBounds(knobArea.reduced(padding));
        }

        // Filter display box
        // filterDisplay.setBounds (filterSection.reduced(padding));

        // Toggle buttons (2 columns of 4, square, vertically centred on the *circular knob*
        // itself - i.e. the knob row minus its bottom label strip)
        const int filterStartY = filterTogglesArea.getY()
                               + titleHeight
                               + (knobAreaHeight - knobLabelHeight - toggleGridH) / 2;
        for (int i = 0; i < oscComponents; ++i)
        {
            int col = i / 4; //TODO: hard coded in rn
            int row = i % 4;
            juce::Rectangle<int> cell (filterTogglesArea.getX() + col * toggleSize,
                                       filterStartY + row * toggleSize,
                                       toggleSize, toggleSize);
            filterToggles[i].setBounds (cell.reduced (toggleSpacing / 2));
        }

        // AMP SECTION
        auto ampTogglesArea = ampSection.removeFromRight (toggleAreaWidth);

        // Title + Knobs
        // auto ampTopRow = ampSection.removeFromTop (knobAreaHeight);
        // ampControlTitle.setBounds (ampTopRow.removeFromLeft (120).withHeight (titleHeight));
        //
        // int ampKnobW = ampTopRow.getWidth() / ampParams;
        // for (int i = 0; i < ampParams; ++i)
        // {
        //     auto knobArea = ampTopRow.removeFromLeft (ampKnobW);
        //     ampSliders[i].setBounds (knobArea.reduced (padding));
        //     ampLabels[i].setBounds  (knobArea.removeFromBottom (20));
        // }

        ampControlTitle.setBounds(ampSection.removeFromTop(titleHeight));
        auto ampTopRow = ampSection.removeFromTop(knobAreaHeight);
        int ampKnobW = ampTopRow.getWidth() / ampParams;
        for (int i = 0; i < ampParams; ++i)
        {
            auto knobArea = ampTopRow.removeFromLeft(ampKnobW);
            ampLabels[i].setBounds(knobArea.removeFromBottom(knobLabelHeight));
            ampSliders[i].setBounds(knobArea.reduced(padding));
        }


        // Amp display box
        // ampDisplay.setBounds (ampSection.reduced (padding));

        // Toggle buttons (2 columns of 4, square, vertically centred on the *circular knob*
        // itself - i.e. the knob row minus its bottom label strip)
        const int ampStartY = ampTogglesArea.getY()
                            + titleHeight
                            + (knobAreaHeight - knobLabelHeight - toggleGridH) / 2;
        for (int i = 0; i < oscComponents; ++i)
        {
            int col = i / 4; //TODO: hard coded in rn
            int row = i % 4;
            juce::Rectangle<int> cell (ampTogglesArea.getX() + col * toggleSize,
                                       ampStartY + row * toggleSize,
                                       toggleSize, toggleSize);
            ampToggles[i].setBounds (cell.reduced (toggleSpacing / 2));
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
        //TODO
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
