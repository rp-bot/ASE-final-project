#pragma once

#include <array>
#include <memory>
#include <juce_audio_basics/juce_audio_basics.h>
#include "../DSP/Oscillator.h"
#include "../DSP/WavetableOscillator.h"
#include "../DSP/WavetableBank.h"
#include "../DSP/TrilinearMixer8.h"
#include "../DSP/AmpEnvelope.h"
#include "../Threading/AtomicGuiState.h"

namespace Audio
{
    class SynthSound;

    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        SynthVoice(DSP::WavetableBank* wavetableBank, Threading::AtomicGuiState* guiState);
        ~SynthVoice() override;

        bool canPlaySound(juce::SynthesiserSound* sound) override;
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound,
                      int currentPitchWheelPosition) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int newPitchWheelValue) override;
        void controllerMoved(int controllerNumber, int newControllerValue) override;
        void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

        /** Call once when sample rate or block size changes. */
        void prepare(double sampleRate, int blockSize);

    private:
        void updateOscillatorFrequency();

        std::array<std::unique_ptr<DSP::Oscillator>, 8> m_oscillators;
        DSP::TrilinearMixer8 m_mixer;
        DSP::AmpEnvelope m_envelope;

        int m_midiNote = 0;
        bool m_isActive = false;
        double m_sampleRate = 0.0;
        float m_currentVelocity = 0.0f;
        int m_currentPitchWheelPosition = 8192;  // centre

        juce::AudioBuffer<float> m_oscillatorOutputs;
        juce::AudioBuffer<float> m_mixerOutput;
    };
}
