#include "UnisonOscillator.h"
#include <cmath>        // for detune
#include <algorithm>    // for std::clamp

namespace DSP {

    void UnisonOscillator::setFrequency(float freq) {
        frequency = freq;
		// Update all voices with the new frequency
        for (auto& voice : voices) {
            if (voice) {
                voice->setFrequency(freq);
            }
        }
    }

    void UnisonOscillator::setNumVoices(int n) {
        numVoices = std::max(1, n); // at least 1 voice 

        voices.resize(numVoices);

        // create Oscillator instances
        for (auto& voice : voices) {
            if (!voice) {
                voice = std::make_unique<WavetableOscillator>();
                voice->prepare(sampleRate);
                voice->setFrequency(frequency);
            }
        }
    }

    void UnisonOscillator::setDetune(float Amount) {
        detune = Amount;

		if (numVoices <= 1) return; // No detune if only one voice

        float step = detune / (numVoices - 1);
        float start = -detune / 2.0f;

        for (int i = 0; i < numVoices; ++i) {
            float voiceFreq = frequency * std::pow(2.0f, start / 1200.0f);
            voices[i]->setFrequency(voiceFreq);
            start += step;
        }
    }

    void UnisonOscillator::setStereoSpread(float Amount) {
		// This is a placeholder.
    }

    void UnisonOscillator::prepare(float sr) {
        sampleRate = sr;
        for (auto& voice : voices) {
            if (voice) {
                voice->prepare(sampleRate);
            }
        }
        setFrequency(frequency);
        setDetune(detune);
        setStereoSpread(stereoSpread);
    }

}