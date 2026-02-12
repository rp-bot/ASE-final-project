#pragma once

#include "VolumetricSynthAudioProcessor.h"

//==============================================================================
class VolumetricSynthEditor  : public juce::AudioProcessorEditor
{
public:
    explicit VolumetricSynthEditor (VolumetricSynthAudioProcessor&);
    ~VolumetricSynthEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VolumetricSynthAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumetricSynthEditor)
};
