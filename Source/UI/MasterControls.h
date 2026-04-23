#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI {

class MasterControls: public juce::Component {

public:

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    MasterControls(juce::AudioProcessorValueTreeState& apvts);
    ~MasterControls() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:

    static constexpr int filterParams = 4;
    static constexpr int ampParams = 6;
    static constexpr int oscComponents = 8;

    void layoutMasterControls(juce::Rectangle<int> area);


    // text labels
    juce::Label filterControlTitle, ampControlTitle;
    std::array<juce::Label, filterParams> filterLabels;
    std::array<juce::Label, ampParams> ampLabels;

    // sliders
    std::array<juce::Slider, filterParams> filterSliders;
    std::array<juce::Slider, ampParams> ampSliders;

    // visualizers


    // toggle buttons
    std::array<juce::ToggleButton, oscComponents> filterToggles;
    std::array<juce::ToggleButton, oscComponents> ampToggles;

    // apvt attachment (for param update)
    std::array<juce::unique_ptr<SliderAttachment>, filterParams> filterParamsAttachments;
    std::array<juce::unique_ptr<ButtonAttachment>, oscComponents> filterToggleAttachments;

    std::array<juce::unique_ptr<SliderAttachment>, ampParams> ampAttachments;
    std::array<juce::unique_ptr<ButtonAttachment>, oscComponents> ampToggleAttachments;


};

}