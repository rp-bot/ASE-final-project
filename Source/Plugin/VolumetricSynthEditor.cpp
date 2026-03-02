#include "VolumetricSynthAudioProcessor.h"
#include "VolumetricSynthEditor.h"

//==============================================================================
VolumetricSynthEditor::VolumetricSynthEditor (VolumetricSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Seed processor-side GUI state from the message thread.
    processorRef.setGuiCursorPosition (0.5f, 0.5f, 0.5f);
    processorRef.setGuiTrajectoryActive (false);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

VolumetricSynthEditor::~VolumetricSynthEditor()
{
}

//==============================================================================
void VolumetricSynthEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Volumetric Synth", getLocalBounds(), juce::Justification::centred, 1);
}

void VolumetricSynthEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
