#pragma once

#include "UI/Widgets/CursorJoystick2D.h"
#include "UI/Widgets/LabelledKnob.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class CenterControlPanel : public juce::Component
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    explicit CenterControlPanel (juce::AudioProcessorValueTreeState& apvts);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    static void configureCursorKnob (LabelledKnob& knob, const juce::String& text);
    static void configureSectionLabel (juce::Label& label, const juce::String& text);
    static void configureSceneSlider (juce::Slider& slider);
    static void configureSolidToggleButton (juce::Button& button);

    juce::Label sceneSectionLabel_;
    juce::Label cursorSectionLabel_;

    juce::Slider zoomSlider_;
    juce::Label zoomLabel_;
    juce::TextButton gizmoButton_ { "Gizmo" };

    LabelledKnob xKnob;
    LabelledKnob yKnob;
    LabelledKnob zKnob;
    CursorJoystick2D xyJoystick;

    juce::TextButton zeroGButton_ { "Zero G" };

    std::unique_ptr<SliderAttachment> zoomAttachment_;
    std::unique_ptr<ButtonAttachment> gizmoAttachment_;
    std::unique_ptr<SliderAttachment> cursorXAttachment;
    std::unique_ptr<SliderAttachment> cursorYAttachment;
    std::unique_ptr<SliderAttachment> cursorZAttachment;
    std::unique_ptr<ButtonAttachment> zeroGAttachment_;

    int sceneDividerY_ { -1 };
    int cursorDividerY_ { -1 };
};
} // namespace UI
