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

    // LookAndFeel override that suppresses the mouse-hover highlight on the
    // toggle buttons (click-down feedback is preserved).
    class NoHoverButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                   const juce::Colour& backgroundColour,
                                   bool /*shouldDrawButtonAsHighlighted*/,
                                   bool shouldDrawButtonAsDown) override
        {
            juce::LookAndFeel_V4::drawButtonBackground (g, button, backgroundColour,
                                                        false, shouldDrawButtonAsDown);
        }
    };

    juce::Colour accent {juce::Colours::white};

    static constexpr int filterParams = 4;
    static constexpr int ampParams = 6;
    static constexpr int oscComponents = 8;

    void layoutMasterControls(juce::Rectangle<int> area);
    void configureRotarySlider(juce::Slider& slider, juce::Label& label, const juce::String& text);

    void propagateFilterMaster (int paramIndex);
    void propagateAmpMaster (int paramIndex);
    void seedCornerFromMaster (int corner, bool isFilter);
    static void writeNormalised (juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterId,float realValue);

    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    // juce::String getFilterParameterId (int paramIndex) const;
    // juce::String getAmpParameterId (int paramIndex) const;

    // text labels
    juce::Label filterControlTitle, ampControlTitle;
    std::array<juce::Label, filterParams> filterLabels;
    std::array<juce::Label, ampParams> ampLabels;

    // sliders
    std::array<juce::Slider, filterParams> filterSliders;
    std::array<juce::Slider, ampParams> ampSliders;

    // visualizers


    // toggle buttons (TextButton in togglesState mode; gives us a coloured
    // "lit when on" look that honours buttonColourId / buttonOnColourId)
    std::array<juce::TextButton, oscComponents> filterToggles;
    std::array<juce::TextButton, oscComponents> ampToggles;
    NoHoverButtonLookAndFeel toggleLookAndFeel;

    // apvt attachment (for param update)
    std::array<std::unique_ptr<SliderAttachment>, filterParams> filterParamsAttachments;
    std::array<std::unique_ptr<ButtonAttachment>, oscComponents> filterToggleAttachments;

    std::array<std::unique_ptr<SliderAttachment>, ampParams> ampAttachments;
    std::array<std::unique_ptr<ButtonAttachment>, oscComponents> ampToggleAttachments;


};

}