#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{

class OutputSection : public juce::Component,
                      public juce::Timer
{
public:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    explicit OutputSection (juce::AudioProcessorValueTreeState& apvts);
    ~OutputSection() override;

    // Call this from the audio thread-safe meter update mechanism
    // (e.g. from AtomicGuiState or a timer polling atomic floats)
    void setMeterLevels (float leftDb, float rightDb);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    static void configureGainKnob (juce::Slider& slider, juce::Label& label);
    static void configurePanKnob (juce::Slider& slider, juce::Label& label);

    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    // Controls
    juce::Slider gainKnob;
    juce::Slider panSlider;
    juce::Label  faderLabel;
    juce::Label  panLabel;

    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> panAttachment;

    // Meter state (written by audio thread via setMeterLevels, read on message thread)
    std::atomic<float> meterLeftDb  { -96.0f };
    std::atomic<float> meterRightDb { -96.0f };

    float displayLeftDb  { -96.0f };
    float displayRightDb { -96.0f };

    // dB readout labels
    juce::Label dbReadoutLabel;
};

} // namespace UI