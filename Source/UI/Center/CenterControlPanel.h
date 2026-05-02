#pragma once

#include "UI/Widgets/CursorJoystick2D.h"
#include "UI/Widgets/LabelledKnob.h"
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class CenterControlPanel : public juce::Component,
                           private juce::AudioProcessorValueTreeState::Listener
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CursorChangedCallback = std::function<void (float, float, float)>;

    explicit CenterControlPanel (juce::AudioProcessorValueTreeState& apvts);
    ~CenterControlPanel() override;

    void setCursorChangedCallback (CursorChangedCallback callback);

    void setCursorPosition (float x, float y, float z);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    static void configureCursorKnob (LabelledKnob& knob, const juce::String& text);
    static void configureHeightSlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void updateCursorFromControls();
    void updateReadoutFromParams();

    /** Engine Y <-> height slider and Z knob stay in sync (same parameter). */
    void syncEngineYControls (float normalizedY, bool fromHeightSlider);

    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    LabelledKnob xKnob;
    LabelledKnob yKnob;
    LabelledKnob zKnob;
    juce::Slider heightSlider;
    juce::Label heightLabel;
    CursorJoystick2D xyJoystick;

    std::unique_ptr<SliderAttachment> cursorXAttachment;
    std::unique_ptr<SliderAttachment> cursorYAttachment;
    std::unique_ptr<SliderAttachment> cursorZAttachment;

    bool syncingEngineY_ { false };

    CursorChangedCallback onCursorChanged;
};
} // namespace UI
