#include "VolumetricSynthAudioProcessor.h"
#include "VolumetricSynthEditor.h"

//==============================================================================
VolumetricSynthEditor::VolumetricSynthEditor (VolumetricSynthAudioProcessor& p)
    : AudioProcessorEditor (&p)
{
    workspace_ = std::make_unique<UI::SynthEditorWorkspace> (p);
    addAndMakeVisible (*workspace_);

    setResizable (true, true);
    if (auto* c = getConstrainer())
    {
        c->setMinimumWidth (1000);
        c->setMinimumHeight (640);
    }

    startTimerHz (25);
    setSize (1220, 880);
}

VolumetricSynthEditor::~VolumetricSynthEditor() = default;

//==============================================================================
void VolumetricSynthEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void VolumetricSynthEditor::resized()
{
    workspace_->setBounds (getLocalBounds());
}

void VolumetricSynthEditor::timerCallback()
{
    workspace_->tick();
}
