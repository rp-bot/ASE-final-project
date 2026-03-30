#include "VolumetricSynthAudioProcessor.h"

#include <unistd.h>

#include "VolumetricSynthEditor.h"
#include "Parameters/ParameterIDs.h"
#include "Utils/ScopedDenormals.h"
#include "Audio/SynthEngine.h"
#include "IO/MidiManager.h"

//==============================================================================
VolumetricSynthAudioProcessor::VolumetricSynthAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameterManager (*this),
       synthEngine (std::make_unique<Audio::SynthEngine> (&atomicGuiState)),
       midiManager (std::make_unique<IO::MidiManager> (*synthEngine))
{
    auto& apvts = parameterManager.getAPVTS();

    // Cursor listeners
    apvts.addParameterListener (ParameterIDs::cursorX, this);
    apvts.addParameterListener (ParameterIDs::cursorY, this);
    apvts.addParameterListener (ParameterIDs::cursorZ, this);

    // Corner parameter listeners (all 8 corners, 6 params each)
    for (int i = 0; i < 8; ++i)
    {
        apvts.addParameterListener (ParameterIDs::cornerLevel(i), this);
        apvts.addParameterListener (ParameterIDs::cornerDetune(i), this);
        apvts.addParameterListener (ParameterIDs::cornerWaveform(i), this);
        apvts.addParameterListener (ParameterIDs::cornerCoarse(i), this);
        apvts.addParameterListener (ParameterIDs::cornerFine(i), this);
        apvts.addParameterListener (ParameterIDs::cornerPan(i), this);
    }

    // syncCursorParamsToGuiState();
    // syncCornerParamsToGuiState();
    syncParamsToGuiState();
}

VolumetricSynthAudioProcessor::~VolumetricSynthAudioProcessor()
{
    auto& apvts = parameterManager.getAPVTS();

    apvts.removeParameterListener (ParameterIDs::cursorX, this);
    apvts.removeParameterListener (ParameterIDs::cursorY, this);
    apvts.removeParameterListener (ParameterIDs::cursorZ, this);

    for (int i = 0; i < 8; ++i) //TODO: global
    {
        apvts.removeParameterListener (ParameterIDs::cornerLevel(i), this);
        apvts.removeParameterListener (ParameterIDs::cornerDetune(i), this);
        apvts.removeParameterListener (ParameterIDs::cornerWaveform(i), this);
        apvts.removeParameterListener (ParameterIDs::cornerCoarse(i), this);
        apvts.removeParameterListener (ParameterIDs::cornerFine(i), this);
        apvts.removeParameterListener (ParameterIDs::cornerPan(i), this);
    }
}


void VolumetricSynthAudioProcessor::syncCursorParamsToGuiState()
{
    auto& apvts = parameterManager.getAPVTS();
    auto* px = apvts.getRawParameterValue (ParameterIDs::cursorX);
    auto* py = apvts.getRawParameterValue (ParameterIDs::cursorY);
    auto* pz = apvts.getRawParameterValue (ParameterIDs::cursorZ);
    const float x = (px != nullptr) ? px->load() : 0.5f;
    const float y = (py != nullptr) ? py->load() : 0.5f;
    const float z = (pz != nullptr) ? pz->load() : 0.5f;
    atomicGuiState.setCursorPosition (x, y, z);
}

void VolumetricSynthAudioProcessor::syncCornerParamsToGuiState()
{
    DBG("inside corner params"); //NOT UPDATED, ACTIVATED

    auto& apvts = parameterManager.getAPVTS();

    for (int i = 0; i < 8; ++i) //TODO: GLOBAL
    {
        CornerParams params;

        auto* pw = apvts.getRawParameterValue(ParameterIDs::cornerWaveform(i));
        auto* pl = apvts.getRawParameterValue(ParameterIDs::cornerLevel(i));
        auto* pd = apvts.getRawParameterValue(ParameterIDs::cornerDetune(i));
        auto* pc = apvts.getRawParameterValue(ParameterIDs::cornerCoarse(i));
        auto* pf = apvts.getRawParameterValue(ParameterIDs::cornerFine(i));
        auto* pp = apvts.getRawParameterValue(ParameterIDs::cornerPan(i));


        params.waveform = pw
            ? static_cast<DSP::WaveformType>(static_cast<int>(pw->load())) //TODO: BUG: NOT LOADE
            : DSP::WaveformType::Sine;

        params.level  = pl ? pl->load() : 0.0f;
        params.detune = pd ? pd->load() : 0.0f;
        params.coarse = pc ? pc->load() : 0.0f;
        params.fine   = pf ? pf->load() : 0.0f;
        params.pan    = pp ? pp->load() : 0.0f;

        DBG(params.level);
        DBG(params.detune); //READING PROPERLY

        atomicGuiState.setCorner(i, params);
    }
}

void VolumetricSynthAudioProcessor::syncParamsToGuiState() {
    syncCornerParamsToGuiState();
    syncCursorParamsToGuiState();
}

void VolumetricSynthAudioProcessor::setGuiCursorPosition (float x, float y, float z) noexcept
{
    atomicGuiState.setCursorPosition (x, y, z);
}

void VolumetricSynthAudioProcessor::setGuiCursorPosition (glm::vec3 position) noexcept
{
    atomicGuiState.setCursorPosition (position);
}

void VolumetricSynthAudioProcessor::setGuiTrajectoryActive (bool active) noexcept
{
    atomicGuiState.setTrajectoryActive (active);
}

glm::vec3 VolumetricSynthAudioProcessor::getGuiCursorPosition() const noexcept
{
    return atomicGuiState.getCursorPosition();
}

bool VolumetricSynthAudioProcessor::isGuiTrajectoryActive() const noexcept
{
    return atomicGuiState.isTrajectoryActive();
}

int VolumetricSynthAudioProcessor::getLastMidiNote() const noexcept
{
    return synthEngine != nullptr ? synthEngine->getLastMidiNote() : -1;
}

float VolumetricSynthAudioProcessor::getLastVelocity() const noexcept
{
    return synthEngine != nullptr ? synthEngine->getLastVelocity() : 0.0f;
}

int VolumetricSynthAudioProcessor::getLastPitchWheel() const noexcept
{
    return synthEngine != nullptr ? synthEngine->getLastPitchWheel() : 0;
}

int VolumetricSynthAudioProcessor::getLastController() const noexcept
{
    return synthEngine != nullptr ? synthEngine->getLastController() : -1;
}

int VolumetricSynthAudioProcessor::getLastControllerValue() const noexcept
{
    return synthEngine != nullptr ? synthEngine->getLastControllerValue() : 0;
}

//==============================================================================
const juce::String VolumetricSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VolumetricSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VolumetricSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VolumetricSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VolumetricSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VolumetricSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VolumetricSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VolumetricSynthAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String VolumetricSynthAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void VolumetricSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void VolumetricSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (synthEngine != nullptr)
        synthEngine->prepare (sampleRate, samplesPerBlock);
}

void VolumetricSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool VolumetricSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void VolumetricSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    Utils::ScopedDenormals scopedDenormals;

    // Read GUI snapshot once per block (cursor drives mixer gains in SynthVoice).
    const auto cursorPosition = getGuiCursorPosition();
    const auto trajectoryActive = isGuiTrajectoryActive();
    juce::ignoreUnused (cursorPosition, trajectoryActive);

    buffer.clear();

    if (synthEngine != nullptr)
        synthEngine->processBlock (buffer, midiMessages);
}

//==============================================================================
bool VolumetricSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VolumetricSynthAudioProcessor::createEditor()
{
    return new VolumetricSynthEditor (*this);
}

//==============================================================================
void VolumetricSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    const auto state = parameterManager.getAPVTS().copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void VolumetricSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xmlState = getXmlFromBinary (data, sizeInBytes))
    {
        const auto valueTree = juce::ValueTree::fromXml (*xmlState);
        if (valueTree.isValid())
        {
            parameterManager.getAPVTS().replaceState (valueTree);
            // syncCursorParamsToGuiState();
            syncParamsToGuiState(); //called
        }
    }
}

void VolumetricSynthAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue) {
    syncParamsToGuiState();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VolumetricSynthAudioProcessor();
}
