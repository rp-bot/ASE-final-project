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
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
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
    static void configureSectionLabel (juce::Label& label, const juce::String& text);
    static void configureSceneSlider (juce::Slider& slider);
    static void configureSolidToggleButton (juce::Button& button);
    void updateCursorFromControls();
    void updateReadoutFromParams();
    void syncEngineYControls (float normalizedY, bool fromHeightSlider);

    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    juce::Label sceneSectionLabel_;
    juce::Label cursorSectionLabel_;

    juce::Slider zoomSlider_;
    juce::Label zoomLabel_;
    juce::TextButton gizmoButton_ { "Gizmo" };

    LabelledKnob xKnob;
    LabelledKnob yKnob;
    LabelledKnob zKnob;
    juce::Slider heightSlider;
    juce::Label heightLabel;
    CursorJoystick2D xyJoystick;

    juce::TextButton zeroGButton_ { "Zero G" };

    std::unique_ptr<SliderAttachment> zoomAttachment_;
    std::unique_ptr<ButtonAttachment> gizmoAttachment_;
    std::unique_ptr<SliderAttachment> cursorXAttachment;
    std::unique_ptr<SliderAttachment> cursorYAttachment;
    std::unique_ptr<SliderAttachment> cursorZAttachment;
    std::unique_ptr<ButtonAttachment> zeroGAttachment_;

    bool syncingEngineY_ { false };
    int sceneDividerY_ { -1 };
    int cursorDividerY_ { -1 };

    CursorChangedCallback onCursorChanged;
};
} // namespace UI
