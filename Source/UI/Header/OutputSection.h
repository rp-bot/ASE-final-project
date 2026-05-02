#pragma once
#include "UI/Widgets/LabelledKnob.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{

    class OutputSection : public juce::Component,
                          public juce::Timer
    {
    public:
        using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

        explicit OutputSection(juce::AudioProcessorValueTreeState &apvts, std::function<void()> onResetEngineHardOff);
        ~OutputSection() override;

        // Call this from the audio thread-safe meter update mechanism
        // (e.g. from AtomicGuiState or a timer polling atomic floats)
        void setMeterLevels(float leftDb, float rightDb);
        void setEngineHardOffState(bool isHardOff);

        void paint(juce::Graphics &g) override;
        void resized() override;

    private:
        void timerCallback() override;

        static void configureVolumeKnob(LabelledKnob &knob);
        static void configurePanKnob(LabelledKnob &knob);

        juce::AudioProcessorValueTreeState *apvtsPtr{nullptr};
        std::function<void()> onResetEngineHardOff_;

        // Controls
        LabelledKnob volumeKnob;
        LabelledKnob panKnob;
        juce::TextButton resetEngineButton{"Reset Engine"};

        std::unique_ptr<SliderAttachment> gainAttachment;
        std::unique_ptr<SliderAttachment> panAttachment;

        // Meter state (written by audio thread via setMeterLevels, read on message thread)
        std::atomic<float> meterLeftDb{-96.0f};
        std::atomic<float> meterRightDb{-96.0f};

        float displayLeftDb{-96.0f};
        float displayRightDb{-96.0f};

        // dB readout labels
        juce::Label dbReadoutLabel;
    };

} // namespace UI