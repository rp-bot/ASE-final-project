#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

namespace DSP
{
    enum class FilterMode
    {
        LowPass,
        HighPass,
        BandPass,
        Notch
    };

    enum class FilterSlope
    {
        Slope12dB,  // 12 dB per octave
        Slope24dB   // 24 dB per octave
    };

    class DualFilter
    {
    public:
        DualFilter();
        ~DualFilter() = default;

        // Prepare filter with sample rate and block size
        void prepare(const juce::dsp::ProcessSpec& spec);
        void prepare(double sampleRate, int blockSize);

        // Process audio block
        void processBlock(juce::AudioBuffer<float>& buffer);
        void processBlock(juce::dsp::AudioBlock<float>& block);

        // Set cutoff frequency (Hz)
        void setCutoff(float cutoffHz);
        void setResonance(float resonance);

        // Set filter modes and configuration
        void setModes(FilterMode mode1, FilterMode mode2, bool serialMode);
        void setMode1(FilterMode mode);
        void setMode2(FilterMode mode);
        void setSerialMode(bool serial);

        // Set filter slopes
        void setSlopes(FilterSlope slope1, FilterSlope slope2);
        void setSlope1(FilterSlope slope);
        void setSlope2(FilterSlope slope);

        // Reset filter state
        void reset();

        // Get current configuration
        FilterMode getMode1()       const { return m_mode1; }
        FilterMode getMode2()       const { return m_mode2; }
        bool       isSerialMode()   const { return m_serialMode; }
        float      getCutoff()      const { return m_cutoffHz; }
        float      getResonance()   const { return m_resonance; }

    private:

        juce::dsp::LadderFilter<float> m_filter1;
        juce::dsp::LadderFilter<float> m_filter2;

        FilterMode m_mode1{ FilterMode::LowPass };
        FilterMode m_mode2{ FilterMode::LowPass };

        FilterSlope m_slope1{ FilterSlope::Slope24dB };
        FilterSlope m_slope2{ FilterSlope::Slope24dB };

        bool m_serialMode{ true };

        float m_cutoffHz{ 1000.0f };
        float m_resonance{ 0.1f };

        double m_sampleRate{ 44100.0 };
        int m_blockSize{ 512 };
        int m_numChannels{ 2 };
        bool m_isPrepared{ false };

        juce::AudioBuffer<float> m_parallelBuffer;
        juce::AudioBuffer<float> m_notchScratchBuffer;

        void updateFilter1();
        void updateFilter2();
        void processSingleFilter(juce::dsp::AudioBlock<float>& block,
                                 juce::dsp::LadderFilter<float>& filter,
                                 FilterMode mode,
                                 FilterSlope slope);

        static juce::dsp::LadderFilterMode convertMode(FilterMode mode, FilterSlope slope);
    };
}