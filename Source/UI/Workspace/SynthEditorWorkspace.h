#pragma once

#include "Plugin/VolumetricSynthAudioProcessor.h"
#include "UI/Center/CenterControlPanel.h"
#include "UI/Envelope/AmpEnvelopeSection.h"
#include "UI/Filter/FilterSection.h"
#include "UI/Header/TopBar.h"
#include "UI/Oscillator/OscillatorModuleComponent.h"
#include "Visualization/GLContextHost.h"
#include "Visualization/Renderer3D.h"
#include <array>
#include <glm/glm.hpp>
#include <memory>

namespace UI
{

class SynthEditorWorkspace;

/** Forwards mouse events to SynthEditorWorkspace for 3D interaction. */
class GLViewportComponent : public juce::Component
{
public:
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override;

private:
    SynthEditorWorkspace* getWorkspace() const noexcept;
};

/** Main plugin UI content: banks, center column, GL view, and bottom strips. */
class SynthEditorWorkspace : public juce::Component
{
public:
    static constexpr int modulesPerBank = 4;

    explicit SynthEditorWorkspace (VolumetricSynthAudioProcessor& processor);
    ~SynthEditorWorkspace() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

    void tick();

    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
    void updateCursorParametersFromPosition (glm::vec3 position);

    VolumetricSynthAudioProcessor& processorRef;

    std::array<std::unique_ptr<OscillatorModuleComponent>, modulesPerBank> leftModules;
    std::array<std::unique_ptr<OscillatorModuleComponent>, modulesPerBank> rightModules;
    std::unique_ptr<CenterControlPanel> centerPanel;
    std::unique_ptr<TopBar> topBar;
    std::unique_ptr<AmpEnvelopeSection> ampSection;
    std::unique_ptr<FilterSection> filterSection;

    GLViewportComponent glViewport_;
    Visualization::GLContextHost glContextHost_;
    Visualization::Renderer3D renderer3D_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthEditorWorkspace)
};

} // namespace UI
