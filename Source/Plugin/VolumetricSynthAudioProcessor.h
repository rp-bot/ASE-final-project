#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <memory>
#include "../Parameters/ParameterManager.h"
#include "../Parameters/ParameterCorners.h"
#include "../Threading/AtomicGuiState.h"

namespace Audio
{
    class SynthEngine;
}

namespace IO
{
    class MidiManager;
}

//==============================================================================
class VolumetricSynthAudioProcessor  : public juce::AudioProcessor,
                                      private juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    VolumetricSynthAudioProcessor();
    ~VolumetricSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    Audio::SynthEngine& getSynthEngine() noexcept;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    ParameterManager& getParameterManager() { return parameterManager; }

    // ---- GUI state bridge (message thread write / audio thread read) ----
    void setGuiCursorPosition (float x, float y, float z) noexcept;
    void setGuiCursorPosition (glm::vec3 position) noexcept;
    void setGuiTrajectoryActive (bool active) noexcept;

    glm::vec3 getGuiCursorPosition() const noexcept;
    bool isGuiTrajectoryActive() const noexcept;

    int getLastMidiNote() const noexcept;
    float getLastVelocity() const noexcept;
    int getLastPitchWheel() const noexcept;
    int getLastController() const noexcept;
    int getLastControllerValue() const noexcept;
    bool isEngineHardOff() const noexcept;
    void resetEngineHardOff() noexcept;

private:
    void parameterChanged (const juce::String& parameterID, float newValue) override;

    void syncParamsToGuiState();
    void syncCursorParamsToGuiState();
    void syncCornerParamsToGuiState();

    ParameterManager parameterManager;
    Threading::AtomicGuiState atomicGuiState;
    std::unique_ptr<Audio::SynthEngine> synthEngine;
    std::unique_ptr<IO::MidiManager> midiManager;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VolumetricSynthAudioProcessor)
};
