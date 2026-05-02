#include "VolumetricSynthAudioProcessor.h"

#include "VolumetricSynthEditor.h"
#include "Parameters/ParameterIDs.h"
#include "Utils/ScopedDenormals.h"
#include "Audio/SynthEngine.h"
#include "IO/MidiManager.h"
#include <cmath>

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
       synthEngine (std::make_unique<Audio::SynthEngine> (&atomicGuiState, &parameterManager.getAPVTS())),
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

    for (auto& bin : spectrumBins)
        bin.store (0.0f, std::memory_order_relaxed);
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

void VolumetricSynthAudioProcessor::setGuiCubeRotation (glm::quat worldFromLocal) noexcept
{
    atomicGuiState.setCubeRotation (worldFromLocal);
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

bool VolumetricSynthAudioProcessor::isEngineHardOff() const noexcept
{
    return synthEngine != nullptr ? synthEngine->isEngineHardOff() : false;
}

void VolumetricSynthAudioProcessor::resetEngineHardOff() noexcept
{
    if (synthEngine != nullptr)
        synthEngine->resetEngineHardOff();
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
    // Host graph sample rate and maximum samples per processBlock callback — propagate through DSP.
    if (synthEngine != nullptr)
        synthEngine->prepare (sampleRate, samplesPerBlock);

    spectrumSampleRateHz.store (static_cast<float> (sampleRate), std::memory_order_relaxed);
    spectrumFifo.fill (0.0f);
    spectrumFFTData.fill (0.0f);
    spectrumFifoIndex = 0;
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

    if (const double sr = getSampleRate(); sr > 0.0)
        spectrumSampleRateHz.store (static_cast<float> (sr), std::memory_order_relaxed);

    // Read GUI snapshot once per block (cursor drives mixer gains in SynthVoice).
    const auto cursorPosition = getGuiCursorPosition();
    const auto trajectoryActive = isGuiTrajectoryActive();
    juce::ignoreUnused (cursorPosition, trajectoryActive);

    buffer.clear();

    if (synthEngine != nullptr)
        synthEngine->processBlock (buffer, midiMessages);

    analyseOutputSpectrum (buffer);
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
    juce::ignoreUnused (parameterID, newValue);
    syncParamsToGuiState();
}


Audio::SynthEngine& VolumetricSynthAudioProcessor::getSynthEngine() noexcept
{
    return *synthEngine;
}

const std::array<std::atomic<float>, VolumetricSynthAudioProcessor::spectrumBinCount>&
VolumetricSynthAudioProcessor::getSpectrumData() const noexcept
{
    return spectrumBins;
}

const std::atomic<float>& VolumetricSynthAudioProcessor::getSpectrumSampleRateHz() const noexcept
{
    return spectrumSampleRateHz;
}

void VolumetricSynthAudioProcessor::analyseOutputSpectrum (const juce::AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = juce::jmax (1, buffer.getNumChannels());
    if (numSamples <= 0)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float mono = 0.0f;
        for (int channel = 0; channel < numChannels; ++channel)
            mono += buffer.getSample (channel, sample);
        mono /= static_cast<float> (numChannels);

        spectrumFifo[static_cast<size_t> (spectrumFifoIndex++)] = mono;
        if (spectrumFifoIndex != spectrumFFTSize)
            continue;

        std::fill (spectrumFFTData.begin(), spectrumFFTData.end(), 0.0f);
        std::copy (spectrumFifo.begin(), spectrumFifo.end(), spectrumFFTData.begin());
        spectrumWindow.multiplyWithWindowingTable (spectrumFFTData.data(), spectrumFFTSize);
        spectrumFFT.performFrequencyOnlyForwardTransform (spectrumFFTData.data());

        const float srStored = spectrumSampleRateHz.load (std::memory_order_relaxed);
        const float srLive = static_cast<float> (getSampleRate());
        const auto sampleRate =
            srLive > 0.0f ? srLive : (srStored > 0.0f ? srStored : 44100.0f);
        const auto nyquist = juce::jmax (1000.0f, sampleRate * 0.5f);
        constexpr float minHz = 20.0f;

        for (size_t bin = 0; bin < spectrumBinCount; ++bin)
        {
            const auto t0 = static_cast<float> (bin) / static_cast<float> (spectrumBinCount);
            const auto t1 = static_cast<float> (bin + 1) / static_cast<float> (spectrumBinCount);
            const auto hz0 = minHz * std::pow (nyquist / minHz, t0);
            const auto hz1 = minHz * std::pow (nyquist / minHz, t1);
            const auto fft0 = juce::jlimit (1, spectrumFFTSize / 2 - 1, static_cast<int> (hz0 * spectrumFFTSize / sampleRate));
            const auto fft1 = juce::jlimit (fft0, spectrumFFTSize / 2 - 1, static_cast<int> (hz1 * spectrumFFTSize / sampleRate));

            float mag = 0.0f;
            for (int i = fft0; i <= fft1; ++i)
                mag = juce::jmax (mag, spectrumFFTData[static_cast<size_t> (i)]);

            const auto db = juce::Decibels::gainToDecibels (mag / static_cast<float> (spectrumFFTSize), -100.0f);
            const auto norm = juce::jlimit (0.0f, 1.0f, juce::jmap (db, -90.0f, 0.0f, 0.0f, 1.0f));
            const auto prev = spectrumBins[bin].load (std::memory_order_relaxed);
            const auto smoothed = juce::jmax (norm, prev * 0.93f);
            spectrumBins[bin].store (smoothed, std::memory_order_relaxed);
        }

        spectrumFifoIndex = 0;
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VolumetricSynthAudioProcessor();
}
