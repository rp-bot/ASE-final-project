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
} //TODO: this could go into the header; copied from OscilatorModuleComponent.cpp for now

namespace UI {
    MasterControls::MasterControls(juce::AudioProcessorValueTreeState &apvts)
    {

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
            //TODO: LINK UP KNOBS
            // filterParamsAttachments[static_cast<size_t> (i)] =
            //     std::make_unique<SliderAttachment> (apvts, , filterSliders[static_cast<size_t> (i)]);
        }

        // Toggle Buttons
        for (int i = 0; i < oscComponents; ++i)
        {
            filterToggles[static_cast<size_t> (i)].setClickingTogglesState (true);
            filterToggles[static_cast<size_t> (i)].setColour (juce::TextButton::buttonColourId,
                                                           accent.withAlpha (0.25f));
            filterToggles[static_cast<size_t> (i)].setColour (juce::TextButton::buttonOnColourId,
                                                           accent.withAlpha (0.9f));
            addAndMakeVisible (filterToggles[static_cast<size_t> (i)]);
            filterToggleAttachments[static_cast<size_t>(i)] =
                std::make_unique<ButtonAttachment>(apvts, "osc_filter_" + juce::String(i) + "_enabled", filterToggles[static_cast<size_t>(i)]);        }

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
            //TODO: LINK UP KNOB TO APVT
            // ampAttachments[static_cast<size_t> (i)] =
            //     std::make_unique<SliderAttachment> (apvts, , ampSliders[static_cast<size_t> (i)]);
        }

        // Toggle Buttons
        for (int i = 0; i < oscComponents; ++i)
        {
            ampToggles[static_cast<size_t> (i)].setClickingTogglesState (true);
            ampToggles[static_cast<size_t> (i)].setColour (juce::TextButton::buttonColourId,
                                                           accent.withAlpha (0.25f));
            ampToggles[static_cast<size_t> (i)].setColour (juce::TextButton::buttonOnColourId,
                                                           accent.withAlpha (0.9f));
            addAndMakeVisible (ampToggles[static_cast<size_t> (i)]);
            ampToggleAttachments[static_cast<size_t>(i)] =
                std::make_unique<ButtonAttachment>(apvts, "osc_amp_" + juce::String(i) + "_enabled", ampToggles[static_cast<size_t>(i)]);
        }

    }

    MasterControls::~MasterControls() {}

    void MasterControls::resized()
    {
        auto bounds = getLocalBounds();

        // Split vertically into two halves
        auto filterSection = bounds.removeFromTop (bounds.getHeight() / 2);
        auto ampSection    = bounds; // remainder

        const int toggleAreaWidth = 50;
        const int titleHeight     = 40;
        const int knobAreaHeight  = 100;
        const int padding         = 1;

        // FILTER SECTOIN
        auto filterTogglesArea = filterSection.removeFromRight (toggleAreaWidth);

        // Title, Knobs (seperate rows)
        filterControlTitle.setBounds(filterSection.removeFromTop(titleHeight));
        auto filterTopRow = filterSection.removeFromTop(knobAreaHeight);
        int knobW = filterTopRow.getWidth() / filterParams;
        for (int i = 0; i < filterParams; ++i)
        {
            auto knobArea = filterTopRow.removeFromLeft(knobW);
            filterLabels[i].setBounds(knobArea.removeFromBottom(20));
            filterSliders[i].setBounds(knobArea.reduced(padding));
        }

        // Filter display box
        // filterDisplay.setBounds (filterSection.reduced(padding));

        // Toggle buttons (2 columns of 4)
        for (int i = 0; i < oscComponents; ++i)
        {
            int col = i / 4; //TODO: hard coded in rn
            int row = i % 4;
            int btnW = filterTogglesArea.getWidth() / 2;
            int btnH = filterTogglesArea.getHeight() / 4;
            filterToggles[i].setBounds (filterTogglesArea.getX() + col * btnW,
                                        filterTogglesArea.getY() + row * btnH,
                                        btnW, btnH);
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
            ampLabels[i].setBounds(knobArea.removeFromBottom(20));
            ampSliders[i].setBounds(knobArea.reduced(padding));
        }


        // Amp display box
        // ampDisplay.setBounds (ampSection.reduced (padding));

        // Toggle buttons (2 columns of 4)
        for (int i = 0; i < oscComponents; ++i)
        {
            int col = i / 4; //TODO: hard coded in rn
            int row = i % 4;
            int btnW = ampTogglesArea.getWidth() / 2;
            int btnH = ampTogglesArea.getHeight() / 4;
            ampToggles[i].setBounds (ampTogglesArea.getX() + col * btnW,
                                     ampTogglesArea.getY() + row * btnH,
                                     btnW, btnH);
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




}
