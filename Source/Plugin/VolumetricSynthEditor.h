#pragma once

#include "VolumetricSynthAudioProcessor.h"
#include "UI/Workspace/SynthEditorWorkspace.h"
#include <memory>

//==============================================================================
class VolumetricSynthEditor : public juce::AudioProcessorEditor,
                             private juce::Timer
{
public:
    explicit VolumetricSynthEditor (VolumetricSynthAudioProcessor&);
    ~VolumetricSynthEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    std::unique_ptr<UI::SynthEditorWorkspace> workspace_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumetricSynthEditor)
};
