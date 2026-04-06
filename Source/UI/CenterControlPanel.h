#pragma once

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
    using TrajectoryChangedCallback = std::function<void (bool)>;

    explicit CenterControlPanel (juce::AudioProcessorValueTreeState& apvts);
    ~CenterControlPanel() override;

    void setCursorChangedCallback (CursorChangedCallback callback);
    void setTrajectoryChangedCallback (TrajectoryChangedCallback callback);

    void setCursorPosition (float x, float y, float z);
    void setTrajectoryActive (bool isActive);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;
    static void configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void updateCursorFromSliders();
    void updateReadoutAndGainsFromParams();

    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };
    juce::Slider xSlider;
    juce::Slider ySlider;
    juce::Slider zSlider;
    juce::Slider gainSlider;
    juce::Label xLabel;
    juce::Label yLabel;
    juce::Label zLabel;
    juce::Label gainLabel;
    juce::ToggleButton trajectoryToggle;
    juce::Label cursorReadoutLabel;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> cursorXAttachment;
    std::unique_ptr<SliderAttachment> cursorYAttachment;
    std::unique_ptr<SliderAttachment> cursorZAttachment;
    CursorChangedCallback onCursorChanged;
    TrajectoryChangedCallback onTrajectoryChanged;
};
} // namespace UI
