#include "SynthVoice.h"
#include "SynthSound.h"
#include "../DSP/FastMath.h"
#include "../Utils/Math3D.h"
#include "../Utils/ScopedDenormals.h"
#include <cmath>

namespace Audio
{
    namespace
    {
        void blendAmpAtCursor(const ParameterSnapshot &snap, float &attack, float &decay, float &sustain,
                              float &release, float &level, float &velSens) noexcept
        {
            const glm::vec3 c = Utils::clampToUnitCube(snap.cursor);
            const auto w = Utils::trilinearWeights(c);
            attack = decay = release = level = velSens = 0.0f;
            sustain = 0.0f;
            for (int i = 0; i < NUM_OSCS; ++i)
            {
                const auto &o = snap.osc[static_cast<size_t>(i)];
                const float wi = w[static_cast<size_t>(i)];
                attack += wi * o.ampAttack;
                decay += wi * o.ampDecay;
                sustain += wi * o.ampSustain;
                release += wi * o.ampRelease;
                level += wi * o.ampLevel;
                velSens += wi * o.ampVelSens;
            }
        }

        float keyTrackedCutoffHz(float baseHz, float keyTrack, int midiNote) noexcept
        {
            const float semis = keyTrack * static_cast<float>(midiNote - 69);
            const float hz = baseHz * std::pow(2.0f, semis / 12.0f);
            return juce::jlimit(40.0f, 19000.0f, hz);
        }

        float resonanceFromNormalized(float q01) noexcept
        {
            return juce::jmap(juce::jlimit(0.0f, 1.0f, q01), 0.5f, 2.2f);
        }

        /** Fewer setCutoffFrequency calls; smoother still advances every sample. */
        constexpr int kFilterCutoffSubBlockSamples = 4;

        template <typename Filter>
        void processDrivenWithFilterSubBlocks(Filter &filtStage1, Filter &filtStage2,
                                              juce::LinearSmoothedValue<float> &smoother, const float *oscRaw,
                                              float *dest, int numSamples, float level, float driveMul, int subBlockSize)
        {
            int s = 0;
            while (s < numSamples)
            {
                const int chunk = juce::jmin(subBlockSize, numSamples - s);
                const auto cutoff = smoother.getNextValue();
                filtStage1.setCutoffFrequency(cutoff);
                filtStage2.setCutoffFrequency(cutoff);
                for (int i = 0; i < chunk; ++i)
                {
                    const float driven = DSP::fastTanh(oscRaw[s + i] * level * driveMul);
                    const auto stage1 = filtStage1.processSample(0, driven);
                    dest[s + i] = filtStage2.processSample(0, stage1);
                    if (i < chunk - 1)
                        (void)smoother.getNextValue();
                }
                s += chunk;
            }
        }

        void processDrivenWithFilterSubBlocksResonant(
            juce::dsp::StateVariableTPTFilter<float> &filtStage1,
            juce::dsp::StateVariableTPTFilter<float> &filtStage2,
            juce::LinearSmoothedValue<float> &cutoffSmoother,
            juce::LinearSmoothedValue<float> &resonanceSmoother,
            const float *oscRaw,
            float *dest,
            int numSamples,
            float level,
            float driveMul,
            int subBlockSize)
        {
            int s = 0;
            while (s < numSamples)
            {
                const int chunk = juce::jmin(subBlockSize, numSamples - s);
                const auto cutoff = cutoffSmoother.getNextValue();
                const auto resonanceNorm = resonanceSmoother.getNextValue();
                const auto resonanceMapped = resonanceFromNormalized(resonanceNorm);

                filtStage1.setCutoffFrequency(cutoff);
                filtStage2.setCutoffFrequency(cutoff);
                filtStage1.setResonance(resonanceMapped);
                filtStage2.setResonance(resonanceMapped);

                for (int i = 0; i < chunk; ++i)
                {
                    const float driven = DSP::fastTanh(oscRaw[s + i] * level * driveMul);
                    const auto stage1 = filtStage1.processSample(0, driven);
                    dest[s + i] = filtStage2.processSample(0, stage1);
                    if (i < chunk - 1)
                    {
                        (void)cutoffSmoother.getNextValue();
                        (void)resonanceSmoother.getNextValue();
                    }
                }
                s += chunk;
            }
        }
    } // namespace

    SynthVoice::SynthVoice(DSP::WavetableBank *wavetableBank)
    {
        for (size_t i = 0; i < 8; ++i)
        {
            m_oscillators[i] = std::make_unique<DSP::WavetableOscillator>(wavetableBank);
            static_cast<DSP::WavetableOscillator *>(m_oscillators[i].get())->setWavetable(static_cast<int>(i));
        }
    }

    SynthVoice::~SynthVoice() = default;

    void SynthVoice::setParameterSnapshot(const ParameterSnapshot *snapshot) noexcept
    {
        m_snapshot = snapshot;
    }

    void SynthVoice::prepare(double sampleRate, int blockSize)
    {
        m_sampleRate = sampleRate;

        m_oscillatorOutputs.setSize(8, blockSize);
        m_oscScratch.setSize(1, blockSize);
        m_mixerOutput.setSize(2, blockSize);

        const juce::dsp::ProcessSpec spec{sampleRate, 1, 1};
        for (size_t i = 0; i < 8; ++i)
        {
            if (m_oscillators[i] != nullptr)
                m_oscillators[i]->prepare(sampleRate);
            m_filters[i].prepare(spec);
            m_filters[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            m_filters[i].setResonance(1.0f / std::sqrt(2.0f));
            m_filtersStage2[i].prepare(spec);
            m_filtersStage2[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);
            m_filtersStage2[i].setResonance(1.0f / std::sqrt(2.0f));
            m_firstOrderFilters[i].prepare(spec);
            m_firstOrderFilters[i].setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
            m_firstOrderFiltersStage2[i].prepare(spec);
            m_firstOrderFiltersStage2[i].setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
            m_cutoffSmoothers[i].reset(sampleRate, 0.02);
            m_cutoffSmoothers[i].setCurrentAndTargetValue(4000.0f);
            m_resonanceSmoothers[i].reset(sampleRate, 0.02);
            m_resonanceSmoothers[i].setCurrentAndTargetValue(0.2f);
        }

        m_mixer.prepare(sampleRate);
        m_envelope.prepare(sampleRate);
        m_envelope.setADSR(0.01f, 0.1f, 0.7f, 0.3f);
    }

    bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound)
    {
        return dynamic_cast<SynthSound *>(sound) != nullptr;
    }

    void SynthVoice::applyAmpEnvelopeFromSnapshot()
    {
        if (m_snapshot == nullptr)
            return;

        float a = 0.01f, d = 0.1f, s = 0.7f, r = 0.3f, lvl = 1.0f, vs = 0.5f;
        blendAmpAtCursor(*m_snapshot, a, d, s, r, lvl, vs);
        m_blendedAmpLevel = juce::jlimit(0.0f, 1.0f, lvl);
        m_blendedVelSens = juce::jlimit(0.0f, 1.0f, vs);
        m_envelope.setADSR(juce::jmax(0.0005f, a), juce::jmax(0.0005f, d), juce::jlimit(0.0f, 1.0f, s),
                           juce::jmax(0.0005f, r));
    }

    void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound * /*sound*/,
                               int currentPitchWheelPosition)
    {
        m_midiNote = midiNoteNumber;
        m_currentVelocity = velocity;
        m_currentPitchWheelPosition = currentPitchWheelPosition;
        m_isActive = true;

        for (auto &osc : m_oscillators)
            if (osc != nullptr)
                osc->reset();

        for (size_t i = 0; i < 8; ++i)
        {
            m_filters[i].reset();
            m_filtersStage2[i].reset();
            m_firstOrderFilters[i].reset();
            m_firstOrderFiltersStage2[i].reset();
            m_useOnePolePrev[i] = false;
            if (m_snapshot != nullptr)
            {
                const auto &o = m_snapshot->osc[i];
                const float fc = keyTrackedCutoffHz(o.filterCutoffHz, o.filterKeyTrack, m_midiNote);
                m_cutoffSmoothers[i].setCurrentAndTargetValue(fc);
                m_resonanceSmoothers[i].setCurrentAndTargetValue(juce::jlimit(0.0f, 1.0f, o.filterResonance));
            }
        }

        applyAmpEnvelopeFromSnapshot();
        m_envelope.noteOn();
    }

    void SynthVoice::stopNote(float /*velocity*/, bool allowTailOff)
    {
        applyAmpEnvelopeFromSnapshot();
        m_envelope.noteOff();
        if (!allowTailOff)
        {
            m_envelope.reset();
            clearCurrentNote();
            m_isActive = false;
        }
    }

    void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
    {
        m_currentPitchWheelPosition = newPitchWheelValue;
    }

    void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
    {
    }

    void SynthVoice::updateOscillatorFrequencies()
    {
        if (m_snapshot == nullptr)
            return;

        const float bendSemitones = 2.0f * static_cast<float>(m_currentPitchWheelPosition - 8192) / 8192.0f;
        const double baseFreq = juce::MidiMessage::getMidiNoteInHertz(m_midiNote);

        for (size_t i = 0; i < 8; ++i)
        {
            if (m_oscillators[i] == nullptr)
                continue;

            const auto &o = m_snapshot->osc[i];
            static_cast<DSP::WavetableOscillator *>(m_oscillators[i].get())
                ->setWavetable(juce::jlimit(0, 3, o.waveform));

            const float totalSemitones = bendSemitones + o.coarse + o.fine + o.detune;
            const float oscFreq =
                static_cast<float>(baseFreq * std::pow(2.0, static_cast<double>(totalSemitones) / 12.0));
            m_oscillators[i]->setFrequency(oscFreq);
        }
    }

    void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples)
    {
        Utils::ScopedDenormals scopedDenormals;

        if (numSamples <= 0 || m_sampleRate <= 0.0)
            return;

        // Internal buffers are sized from prepare(samplesPerBlock). If the host delivers a larger
        // sub-block than the last prepare hint, grow once rather than truncating the callback.
        if (numSamples > m_oscillatorOutputs.getNumSamples() || m_oscScratch.getNumSamples() < numSamples
            || m_mixerOutput.getNumSamples() < numSamples || m_mixerOutput.getNumChannels() < 2)
            prepare(m_sampleRate, numSamples);

        if (numSamples > m_oscillatorOutputs.getNumSamples())
            return;

        if (m_snapshot != nullptr)
        {
            applyAmpEnvelopeFromSnapshot();
            updateOscillatorFrequencies();

            for (size_t i = 0; i < 8; ++i)
            {
                const auto &o = m_snapshot->osc[i];
                const float fc = keyTrackedCutoffHz(o.filterCutoffHz, o.filterKeyTrack, m_midiNote);
                m_cutoffSmoothers[i].setTargetValue(fc);
                m_resonanceSmoothers[i].setTargetValue(juce::jlimit(0.0f, 1.0f, o.filterResonance));
            }
        }

        std::array<float, 8> pan{};
        if (m_snapshot != nullptr)
        {
            for (int i = 0; i < NUM_OSCS; ++i)
                pan[static_cast<size_t>(i)] = m_snapshot->osc[static_cast<size_t>(i)].pan;
        }

        const glm::vec3 cursor = m_snapshot != nullptr ? m_snapshot->cursor : glm::vec3(0.5f, 0.5f, 0.5f);

        // Oscillator-outer: wavetable block fill then drive/filter per sample; level/drive hoisted.
        float *const oscRaw = m_oscScratch.getWritePointer(0);
        for (size_t i = 0; i < 8; ++i)
        {
            auto *osc = m_oscillators[i].get();
            if (osc == nullptr)
                continue;

            float level = 1.0f;
            float driveMul = 1.0f;
            if (m_snapshot != nullptr)
            {
                const auto &o = m_snapshot->osc[i];
                level = o.level;
                driveMul = 1.0f + o.filterDrive * 4.0f;
            }

            osc->processBlock(oscRaw, numSamples);

            float *const dest = m_oscillatorOutputs.getWritePointer(static_cast<int>(i));
            auto &smoother = m_cutoffSmoothers[i];
            auto &resSmoother = m_resonanceSmoothers[i];
            processDrivenWithFilterSubBlocksResonant(m_filters[i], m_filtersStage2[i], smoother, resSmoother, oscRaw,
                                                     dest, numSamples, level, driveMul, kFilterCutoffSubBlockSamples);
        }

        m_mixer.processBlock(m_oscillatorOutputs, m_mixerOutput, 0, numSamples, cursor, pan);

        const int numChannels = juce::jmin(outputBuffer.getNumChannels(), 2);
        constexpr float kHeadroomGain = 0.89f;
        const float velScale =
            juce::jlimit(0.0f, 1.0f, (1.0f - m_blendedVelSens) + m_blendedVelSens * m_currentVelocity);

        for (int s = 0; s < numSamples; ++s)
        {
            const float envVal = m_envelope.processSample();
            const float mixedL = m_mixerOutput.getSample(0, s);
            const float mixedR = m_mixerOutput.getNumChannels() > 1 ? m_mixerOutput.getSample(1, s) : mixedL;
            const float gain = kHeadroomGain * envVal * velScale * m_blendedAmpLevel;

            const float outL = mixedL * gain;
            const float outR = mixedR * gain;

            const int outIndex = startSample + s;
            outputBuffer.addSample(0, outIndex, outL);
            if (numChannels >= 2)
                outputBuffer.addSample(1, outIndex, outR);

            if (!m_envelope.isActive())
            {
                clearCurrentNote();
                m_isActive = false;
                break;
            }
        }
    }
}
