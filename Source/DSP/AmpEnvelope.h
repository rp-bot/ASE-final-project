#pragma once

namespace DSP
{
    enum class EnvelopeStage
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };
    
    class AmpEnvelope
    {
    public:
        AmpEnvelope();
        ~AmpEnvelope() = default;
        
        // Prepare envelope with sample rate
        void prepare(double sampleRate);
        
        // Process one sample, return envelope value [0.0, 1.0]
        float processSample();
        
        // Trigger envelope (note on)
        void noteOn();
        
        // Release envelope (note off)
        void noteOff();
        
        // Set ADSR parameters (time in seconds, sustain level [0.0, 1.0])
        void setADSR(float attackTime, float decayTime, float sustainLevel, float releaseTime);
        
        // Individual parameter setters
        void setAttackTime(float timeSeconds);
        void setDecayTime(float timeSeconds);
        void setSustainLevel(float level);  // [0.0, 1.0]
        void setReleaseTime(float timeSeconds);
        
        // Get current stage
        EnvelopeStage getCurrentStage() const;
        
        // Get current envelope value
        float getCurrentLevel() const;
        
        // Reset envelope to idle state
        void reset();
        
        // Check if envelope is active (not idle)
        bool isActive() const;

    private:
        EnvelopeStage m_currentStage;
        float m_currentLevel;          // Current envelope value [0.0, 1.0]
        
        // ADSR parameters (in seconds)
        float m_attackTime;
        float m_decayTime;
        float m_sustainLevel;          // [0.0, 1.0]
        float m_releaseTime;
        
        // Sample rate and calculated values
        double m_sampleRate;
        int m_attackSamples;
        int m_decaySamples;
        int m_releaseSamples;
        
        // Stage progress counters
        int m_stageSampleCount;
    };
}