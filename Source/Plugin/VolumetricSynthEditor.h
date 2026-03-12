#pragma once

#include "VolumetricSynthAudioProcessor.h"
#include "../UI/CenterControlPanel.h"
#include "../UI/LabeledPanel.h"
#include "../UI/OscillatorModuleComponent.h"
#include <array>

//==============================================================================
class VolumetricSynthEditor  : public juce::AudioProcessorEditor,
                               private juce::Timer  // DEBUG: editor timer used for temporary MIDI monitor repainting
{
public:
    explicit VolumetricSynthEditor (VolumetricSynthAudioProcessor&);
    ~VolumetricSynthEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    static constexpr int modulesPerBank = 4;
    juce::Rectangle<int> topArea;
    juce::Rectangle<int> leftBankArea;
    juce::Rectangle<int> centerArea;
    juce::Rectangle<int> rightBankArea;
    juce::Rectangle<int> bottomArea;
    juce::Rectangle<int> bottomLeftArea;
    juce::Rectangle<int> bottomCenterArea;
    juce::Rectangle<int> bottomRightArea;

    VolumetricSynthAudioProcessor& processorRef;
    std::array<std::unique_ptr<UI::OscillatorModuleComponent>, modulesPerBank> leftModules;
    std::array<std::unique_ptr<UI::OscillatorModuleComponent>, modulesPerBank> rightModules;
    std::unique_ptr<UI::CenterControlPanel> centerPanel;
    UI::LabeledPanel bottomLeftPanel { "Placeholder for Additional Functionalities" };
    UI::LabeledPanel bottomCenterPanel { "Additional Vector Synthesizer parameters" };
    UI::LabeledPanel bottomRightPanel { "Placeholder for Additional Functionalities" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumetricSynthEditor)
};
