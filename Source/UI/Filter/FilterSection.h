#pragma once

#include <array>
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Common/MasterControlsShared.h"
#include "UI/Widgets/FilterResponseEditor.h"

namespace UI
{

// TODO(visage): replace rotary sliders / toggles with library widgets.

/** Bottom-right master filter: response graph, value row, per-oscillator link toggles. */
class FilterSection : public juce::Component,
                      private juce::Timer
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    explicit FilterSection (juce::AudioProcessorValueTreeState& apvts);
    ~FilterSection() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void setSpectrumDataSource (const std::array<std::atomic<float>, 128>* bins,
                                const std::atomic<float>* sampleRateHz);

private:
    static constexpr int filterParams = 4;
    static constexpr int oscComponents = 8;

    void configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void propagateFilterMaster (int paramIndex);
    void seedCornerFromMaster (int corner);
    void syncEditorsFromParameters();
    void commitFilterValueFromEditor (int index);
    void configureValueEditor (MasterDragValueEditor& editor, int index);

    juce::Colour accent { juce::Colours::white };
    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    juce::Label filterControlTitle;
    std::array<juce::Label, filterParams> filterLabels;
    std::array<juce::Slider, filterParams> filterSliders;
    std::array<MasterDragValueEditor, filterParams> filterValueEditors;

    std::unique_ptr<FilterResponseEditor> filterEditor;

    std::array<juce::TextButton, oscComponents> filterToggles;
    NoHoverToggleLookAndFeel toggleLookAndFeel;

    std::array<std::unique_ptr<SliderAttachment>, filterParams> filterParamsAttachments;
    std::array<std::unique_ptr<ButtonAttachment>, oscComponents> filterToggleAttachments;
};

} // namespace UI
