#pragma once

#include "VolumetricSynthAudioProcessor.h"
#include "UI/Common/SynthLookAndFeel.h"
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
    void updateCursorParametersFromPosition (glm::vec3 position);
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp   (const juce::MouseEvent& event) override;
    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
    UI::SynthLookAndFeel synthLAF_;
    std::unique_ptr<UI::SynthEditorWorkspace> workspace_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumetricSynthEditor)
};
