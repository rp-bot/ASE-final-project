#pragma once

#include "Oscillator.h"

namespace DSP {
    class UnisonOscillator : public Oscillator {
    
    public:
        // Override Oscillator class methods
        void setFrequency(float freq) override;

        void setNumVoices(int n);
        void setDetune(float Amount);
        void setStereoSpread(float Amount);
        void prepare(float sampleRate);

    private:
        std::vector<std::unique_ptr<WavetableOscillator>> voices;

        int   numVoices = 1;
        float detune;
        float stereoSpread;
        float frequency = 440.0f;
        float sampleRate = 44100.0f;
    };
}