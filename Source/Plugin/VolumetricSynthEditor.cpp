#include "VolumetricSynthAudioProcessor.h"
#include "VolumetricSynthEditor.h"

//==============================================================================
VolumetricSynthEditor::VolumetricSynthEditor(VolumetricSynthAudioProcessor& p)
    : AudioProcessorEditor(&p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&synthLAF_);

    workspace_ = std::make_unique<UI::SynthEditorWorkspace>(p);
    addAndMakeVisible(*workspace_);

    setResizable(true, true);
    if (auto* c = getConstrainer())
    {
        c->setMinimumWidth(1000);
        c->setMinimumHeight(640);
    }

    startTimerHz(25);
    setSize(1220, 880);
}

VolumetricSynthEditor::~VolumetricSynthEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void VolumetricSynthEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void VolumetricSynthEditor::resized()
{
    workspace_->setBounds(getLocalBounds());
}

void VolumetricSynthEditor::timerCallback()
{
    const auto vpBounds = glViewport_.getBounds();
    // Forward drag even when the pointer leaves the viewport so a gizmo drag
    // that started inside the viewport isn't silently dropped.
    if (vpBounds.contains (event.getPosition()) || renderer3D_.hasActiveDrag())
    {
        renderer3D_.mouseDrag (event, vpBounds);

        const auto newCursor = renderer3D_.getCursorAsUnitPosition();
        processorRef.setGuiCursorPosition (newCursor);
        updateCursorParametersFromPosition (newCursor);
    }
}

void VolumetricSynthEditor::mouseUp (const juce::MouseEvent& event)
{
    renderer3D_.mouseUp (event);
}

void VolumetricSynthEditor::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (glViewport_.getBounds().contains (event.getPosition()))
        renderer3D_.mouseWheelMove (event, wheel);
    workspace_->tick();
}
