#pragma once
#include "Oscillator.h"
#include "WavetableBank.h"

namespace DSP
{
    class WavetableOscillator : public Oscillator
    {
    public:
        explicit WavetableOscillator(WavetableBank* wavetableBank);
        ~WavetableOscillator() override = default;

        // Oscillator interface implementation
        float processSample() override;
        void setFrequency(float frequencyHz) override;
        void setPhase(float phase) override;
        void reset() override;

        // Wavetable-specific methods
        void setWavetable(int index);
        int getCurrentWavetableIndex() const;

        // Sample rate preparation (called before processing)
        void prepare(double sampleRate) override;

        // Get current phase (for visualization/debugging)
        float getPhase() const;

        // Optional: set bank after construction (e.g. when bank is not available at build time)
        void setWavetableBank(WavetableBank* bank);

    private:
        float interpolateWavetable(float phase) const;

        WavetableBank* m_wavetableBank;  // Non-owning pointer (owned elsewhere)
        int m_wavetableIndex = 0;
        float m_phase = 0.0f;           // Current phase [0.0, 1.0)
        float m_phaseIncrement = 0.0f;   // Phase increment per sample
        float m_frequency = 0.0f;       // Current frequency in Hz
        double m_sampleRate = 0.0;      // Sample rate (set by prepare())
    };
}