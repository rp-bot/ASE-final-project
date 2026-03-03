#include <iostream>

float processSample();

float interpolateWavetable(float phase) const;

void setFrequency(float frequencyHz);

private:
    WavetableBank* m_wavetableBank;  // Non-owning pointer (owned elsewhere)
    int m_wavetableIndex;
    float m_phase;                    // Current phase [0.0, 1.0)
    float m_phaseIncrement;           // Phase increment per sample
    float m_frequency;                // Current frequency in Hz
    double m_sampleRate;              // Sample rate (set by prepare())