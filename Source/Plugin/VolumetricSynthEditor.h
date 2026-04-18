#pragma once

#include "VolumetricSynthAudioProcessor.h"
#include "../UI/CenterControlPanel.h"
#include "../UI/LabeledPanel.h"
#include "../UI/OscillatorModuleComponent.h"
#include "../Visualization/GLContextHost.h"
#include "../Visualization/Renderer3D.h"
#include "../UI/OutputSection.h"
#include <glm/glm.hpp>
#include <array>

//==============================================================================
/** Forwards mouse events to its parent so the editor can handle cube picking. */
class GLViewportComponent : public juce::Component
{
public:
    void mouseDown (const juce::MouseEvent& e) override
    {
        if (auto* p = getParentComponent())
            p->mouseDown (e.getEventRelativeTo (p));
    }
    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (auto* p = getParentComponent())
            p->mouseDrag (e.getEventRelativeTo (p));
    }
    void mouseUp (const juce::MouseEvent& e) override
    {
        if (auto* p = getParentComponent())
            p->mouseUp (e.getEventRelativeTo (p));
    }
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override
    {
        if (auto* p = getParentComponent())
            p->mouseWheelMove (e.getEventRelativeTo (p), w);
    }
};

//==============================================================================
class VolumetricSynthEditor  : public juce::AudioProcessorEditor,
                              private juce::Timer
{
public:
    explicit VolumetricSynthEditor (VolumetricSynthAudioProcessor&);
    ~VolumetricSynthEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void updateCursorParametersFromPosition (glm::vec3 position);
    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

    static constexpr int modulesPerBank = 4;
    juce::Rectangle<int> topArea;
    juce::Rectangle<int> leftBankArea;
    juce::Rectangle<int> centerArea;
    juce::Rectangle<int> rightBankArea;
    juce::Rectangle<int> bottomLeftArea;
    juce::Rectangle<int> bottomRightArea;

    VolumetricSynthAudioProcessor& processorRef;
    std::array<std::unique_ptr<UI::OscillatorModuleComponent>, modulesPerBank> leftModules;
    std::array<std::unique_ptr<UI::OscillatorModuleComponent>, modulesPerBank> rightModules;
    std::unique_ptr<UI::CenterControlPanel> centerPanel;
    UI::LabeledPanel bottomLeftPanel { "Placeholder for Additional Functionalities" };
    std::unique_ptr<UI::OutputSection> outputSection;

    GLViewportComponent glViewport_;
    Visualization::GLContextHost glContextHost_;
    Visualization::Renderer3D renderer3D_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumetricSynthEditor)
};
