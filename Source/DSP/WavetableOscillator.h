#pragma once
#include "Oscillator.h"
#include "WavetableBank.h"

namespace DSP
{
    class WavetableOscillator : public Oscillator
    {
    public:
        float processSample() override;
        void setFrequency(float frequencyHz) override;
        void setPhase(float phase) override;
        void prepare(double sampleRate) override;
        void reset() override;

        void setWavetable(int index);
        void setWavetableBank(WavetableBank* bank);

    private:
        float interpolateWavetable(float phase) const;

        WavetableBank* m_wavetableBank = nullptr;  // Non-owning pointer (owned elsewhere)
        int m_wavetableIndex = 0;
        float m_phase = 0.0f;                    // Current phase [0.0, 1.0)
        float m_phaseIncrement = 0.0f;           // Phase increment per sample
        float m_frequency = 0.0f;               // Current frequency in Hz
        double m_sampleRate = 0.0;              // Sample rate (set by prepare())
    };
}