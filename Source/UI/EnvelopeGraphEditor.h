#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class EnvelopeGraphEditor : public juce::Component,
                            private juce::Timer
{
public:
    EnvelopeGraphEditor (juce::AudioProcessorValueTreeState& apvts,
                         std::array<juce::String, 6> parameterIds,
                         juce::Colour accentColour);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    enum class DragTarget
    {
        none,
        attackNode,
        decayNode,
        sustainSegment,
        releaseNode
    };

    void timerCallback() override;
    void refreshFromParameters();
    void writeParameterRealValue (int index, float realValue);

    juce::Rectangle<float> getPlotBounds() const;
    juce::Point<float> getAttackPoint() const;
    juce::Point<float> getDecayPoint() const;
    juce::Point<float> getReleasePoint() const;
    float xToTimeSecondsInRegion (float x, float minX, float maxX, float minTime, float maxTime) const;
    float yToSustainValue (float y) const;

    juce::AudioProcessorValueTreeState& apvts;
    std::array<juce::String, 6> ids;
    juce::Colour accent;
    std::array<float, 6> values { 0.01f, 0.1f, 0.7f, 0.3f, 1.0f, 0.5f };
    DragTarget dragTarget { DragTarget::none };
};
} // namespace UI
