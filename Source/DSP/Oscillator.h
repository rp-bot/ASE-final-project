#pragma once
namespace DSP
{
    class Oscillator
    {
    public:
        virtual ~Oscillator() = default;

        // Generate next sample
        virtual float processSample() = 0;
        
        // Set frequency in Hz
        virtual void setFrequency(float frequencyHz) = 0;
        
        // Set phase (0.0 to 1.0)
        virtual void setPhase(float phase) = 0;
        
        // Reset oscillator state
        virtual void reset() = 0;
    };

}