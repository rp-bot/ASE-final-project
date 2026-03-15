#pragma once

#include "Oscillator.h"
#include "WavetableOscillator.h"
#include "WavetableBank.h"
#include <vector>
#include <memory>

namespace DSP {
    class UnisonOscillator : public Oscillator {
    public:
        // Override Oscillator class methods
        float processSample() override;
        void setFrequency(float freq) override;
        void setPhase(float phase) override;
        void prepare(double sampleRate) override;
        void reset() override;

        void processSample(float& left, float& right);
        void setNumVoices(int n);
        void setDetune(float amount);
        void setStereoSpread(float amount);
        void setWavetable(int index);

        /** Set the wavetable bank used by all voices. Must be called before or when creating voices. */
        void setWavetableBank(WavetableBank* bank);

    private:
        std::vector<std::unique_ptr<Oscillator>> voices;
        WavetableBank* m_wavetableBank = nullptr;

        int   numVoices = 1;
        float detuneAmount = 0.0f;
        float stereoSpread = 0.0f;
        float frequency = 440.0f;
        double sampleRate = 44100.0;
    };
}