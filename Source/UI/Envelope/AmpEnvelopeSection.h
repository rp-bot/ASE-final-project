#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "UI/Common/MasterControlsShared.h"
#include "UI/Widgets/EnvelopeGraphEditor.h"

namespace UI
{

// TODO(visage): replace rotary sliders / toggles with library widgets.

/** Bottom-left master amp envelope: graph, value row, per-oscillator link toggles. */
class AmpEnvelopeSection : public juce::Component,
                           private juce::Timer
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    explicit AmpEnvelopeSection (juce::AudioProcessorValueTreeState& apvts);
    ~AmpEnvelopeSection() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    static constexpr int ampParams = 6;
    static constexpr int oscComponents = 8;

    void configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void propagateAmpMaster (int paramIndex);
    void seedCornerFromMaster (int corner);
    void syncEditorsFromParameters();
    void commitAmpValueFromEditor (int index);
    void configureValueEditor (MasterDragValueEditor& editor, int index);

    juce::Colour accent { juce::Colours::white };
    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    juce::Label ampControlTitle;
    std::array<juce::Label, ampParams> ampLabels;
    std::array<juce::Slider, ampParams> ampSliders;
    std::array<MasterDragValueEditor, ampParams> ampValueEditors;

    std::unique_ptr<EnvelopeGraphEditor> envelopeEditor;

    std::array<juce::TextButton, oscComponents> ampToggles;
    NoHoverToggleLookAndFeel toggleLookAndFeel;

    std::array<std::unique_ptr<SliderAttachment>, ampParams> ampAttachments;
    std::array<std::unique_ptr<ButtonAttachment>, oscComponents> ampToggleAttachments;
};

} // namespace UI
