#pragma once

#include <array>
#include <memory>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "ParameterSnapshot.h"
#include "../DSP/Oscillator.h"
#include "../DSP/WavetableOscillator.h"
#include "../DSP/WavetableBank.h"
#include "../DSP/TrilinearMixer8.h"
#include "../DSP/AmpEnvelope.h"

namespace Audio
{
    class SynthSound;

    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        explicit SynthVoice(DSP::WavetableBank *wavetableBank);
        ~SynthVoice() override;

        void setParameterSnapshot(const ParameterSnapshot *snapshot) noexcept;

        bool canPlaySound(juce::SynthesiserSound *sound) override;
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound,
                       int currentPitchWheelPosition) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void pitchWheelMoved(int newPitchWheelValue) override;
        void controllerMoved(int controllerNumber, int newControllerValue) override;
        void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

        void prepare(double sampleRate, int blockSize);

        /** True from startNote until the envelope tail finishes (see SynthVoice::renderNextBlock). */
        bool isVoiceRendering() const noexcept { return m_isActive; }

    private:
        void applyAmpEnvelopeFromSnapshot();
        void updateOscillatorFrequencies();

        std::array<std::unique_ptr<DSP::Oscillator>, 8> m_oscillators;
        std::array<juce::dsp::StateVariableTPTFilter<float>, 8> m_filters;
        std::array<juce::dsp::StateVariableTPTFilter<float>, 8> m_filtersStage2;
        std::array<juce::dsp::FirstOrderTPTFilter<float>, 8> m_firstOrderFilters;
        std::array<juce::dsp::FirstOrderTPTFilter<float>, 8> m_firstOrderFiltersStage2;
        std::array<juce::LinearSmoothedValue<float>, 8> m_cutoffSmoothers;
        std::array<juce::LinearSmoothedValue<float>, 8> m_resonanceSmoothers;
        std::array<bool, 8> m_useOnePolePrev{};
        DSP::TrilinearMixer8 m_mixer;
        DSP::AmpEnvelope m_envelope;

        const ParameterSnapshot *m_snapshot{nullptr};

        int m_midiNote = 0;
        bool m_isActive = false;
        double m_sampleRate = 0.0;
        float m_currentVelocity = 0.0f;
        int m_currentPitchWheelPosition = 8192;

        float m_blendedAmpLevel = 1.0f;
        float m_blendedVelSens = 0.5f;

        /** Smooths cursor-driven overall note gain to avoid clicks when moving the 3D cursor. */
        juce::LinearSmoothedValue<float> m_noteGainSmoother;
        float m_currentNoteGainBase = 1.0f;

        juce::AudioBuffer<float> m_oscillatorOutputs;
        juce::AudioBuffer<float> m_oscScratch;
        juce::AudioBuffer<float> m_mixerOutput;
    };
}
