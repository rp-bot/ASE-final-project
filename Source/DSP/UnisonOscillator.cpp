#include "UnisonOscillator.h"
#include <cmath>        // for detune
#include <algorithm>    // for std::clamp

namespace DSP {

    void UnisonOscillator::setFrequency(float freq) {
        frequency = freq;
        // Reapply detune so per-voice frequencies stay correct
        if (numVoices > 1 && detuneAmount != 0.0f) {
            setDetune(detuneAmount);
        } else {
            for (auto& voice : voices) {
                if (voice) voice->setFrequency(freq);
            }
        }
    }

    void UnisonOscillator::setWavetableBank(WavetableBank* bank) {
        m_wavetableBank = bank;
        for (auto& voice : voices) {
            auto* wt = dynamic_cast<WavetableOscillator*>(voice.get());
            if (wt) wt->setWavetableBank(bank);
        }
    }

    void UnisonOscillator::setNumVoices(int n) {
        numVoices = std::max(1, n);

        voices.resize(numVoices);

        for (auto& voice : voices) {
            if (!voice) {
                voice = std::make_unique<WavetableOscillator>(m_wavetableBank);
                voice->prepare(sampleRate);
                voice->setFrequency(frequency);
            }
        }
    }

    void UnisonOscillator::setDetune(float amount) {
        detuneAmount = amount;

		if (numVoices <= 1) return; // No detune if only one voice

        float step = detuneAmount / (numVoices - 1);
        float start = -detuneAmount / 2.0f;

        for (int i = 0; i < numVoices; ++i) {
            float voiceFreq = frequency * std::pow(2.0f, start / 1200.0f);
            voices[i]->setFrequency(voiceFreq);
            start += step;
        }
    }

    void UnisonOscillator::setWavetable(int index) {
        for (auto& voice : voices) {
            auto* wt = dynamic_cast<WavetableOscillator*>(voice.get());
            if (wt) wt->setWavetable(index);
        }
    }

    void UnisonOscillator::setStereoSpread(float amount) {
        stereoSpread = amount;
        // Stereo panning is applied in processSample(float&, float&).
    }

    float UnisonOscillator::processSample() {
        float output = 0.0f;
        for (auto& voice : voices) {
            if (voice) output += voice->processSample();
        }
        return numVoices > 0 ? output / numVoices : 0.0f;
    }

    void UnisonOscillator::processSample(float& left, float& right) {
        left = 0.0f;
        right = 0.0f;
        if (numVoices == 0) return;
        for (int i = 0; i < numVoices; ++i) {
            if (!voices[i]) continue;
            float sample = voices[i]->processSample();
            // Distribute voices across the stereo field based on stereoSpread
            float pan = (numVoices > 1)
                ? (static_cast<float>(i) / (numVoices - 1) - 0.5f) * 2.0f * stereoSpread
                : 0.0f;
            float leftGain  = std::clamp(1.0f - pan, 0.0f, 1.0f);
            float rightGain = std::clamp(1.0f + pan, 0.0f, 1.0f);
            left  += sample * leftGain;
            right += sample * rightGain;
        }
        left  /= numVoices;
        right /= numVoices;
    }

    void UnisonOscillator::setPhase(float phase) {
        for (auto& voice : voices) {
            if (voice) voice->setPhase(phase);
        }
    }

    void UnisonOscillator::reset() {
        for (auto& voice : voices) {
            if (voice) voice->reset();
        }
    }

    void UnisonOscillator::prepare(double sr) {
        sampleRate = sr;
        for (auto& voice : voices) {
            if (voice) {
                voice->prepare(sampleRate);
            }
        }
        setFrequency(frequency);
        setDetune(detuneAmount);
        setStereoSpread(stereoSpread);
    }

}