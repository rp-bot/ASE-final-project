#pragma once

#include <array>
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class FilterResponseEditor : public juce::Component,
                             private juce::Timer
{
public:
    FilterResponseEditor (juce::AudioProcessorValueTreeState& apvts,
                          std::array<juce::String, 4> parameterIds,
                          juce::Colour accentColour);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void setSpectrumData (const std::array<std::atomic<float>, 128>* bins,
                          const std::atomic<float>* sampleRateHz);

private:
    enum class DragTarget
    {
        none,
        main,
        keyTrack,
        drive
    };

    void timerCallback() override;
    void refreshFromParameters();
    void writeParameterRealValue (int index, float realValue);

    juce::Rectangle<float> getPlotBounds() const;
    juce::Rectangle<float> getMiniControlBounds (bool forKeyTrack) const;
    juce::Point<float> getMainHandlePosition() const;
    float xToCutoff (float x) const;
    float yToResonance (float y) const;
    float xToMiniValue (float x, bool forKeyTrack) const;

    juce::AudioProcessorValueTreeState& apvts;
    std::array<juce::String, 4> ids;
    juce::Colour accent;
    const std::array<std::atomic<float>, 128>* spectrumBins { nullptr };
    const std::atomic<float>* spectrumSampleRate { nullptr };

    std::array<float, 4> values { 4000.0f, 0.2f, 0.5f, 0.0f };
    DragTarget dragTarget { DragTarget::none };
    juce::Point<float> dragStartPos {};
    float dragStartCutoff { 4000.0f };
    float dragStartResonance { 0.5f };
};
} // namespace UI
