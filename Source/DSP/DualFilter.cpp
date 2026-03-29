#include "DualFilter.h"

namespace DSP
{
    DualFilter::DualFilter() {}

    juce::dsp::LadderFilterMode DualFilter::convertMode(FilterMode mode, FilterSlope slope)
    {
        using Mode = juce::dsp::LadderFilterMode;

        bool is24 = (slope == FilterSlope::Slope24dB);

        switch (mode)
        {
            case FilterMode::LowPass:
                return is24 ? Mode::LPF24 : Mode::LPF12;

            case FilterMode::HighPass:
                return is24 ? Mode::HPF24 : Mode::HPF12;

            case FilterMode::BandPass:
                return is24 ? Mode::BPF24 : Mode::BPF12;

            case FilterMode::Notch:
                // JUCE LadderFilter has no dedicated notch mode in this version.
                // Notch processing uses BPF internally and mixes it as dry - bandpass.
                return is24 ? Mode::BPF24 : Mode::BPF12;
        }

        return Mode::LPF24;
    }

    void DualFilter::prepare(const juce::dsp::ProcessSpec& spec)
    {
        m_sampleRate = spec.sampleRate;
        m_blockSize = (int)spec.maximumBlockSize;
        m_numChannels = (int)spec.numChannels;
        m_isPrepared = true;

        m_filter1.prepare(spec);
        m_filter2.prepare(spec);

        m_parallelBuffer.setSize(
            (int)spec.numChannels,
            (int)spec.maximumBlockSize,
            false,
            false,
            true
        );
        m_notchScratchBuffer.setSize(
            (int)spec.numChannels,
            (int)spec.maximumBlockSize,
            false,
            false,
            true
        );

        updateFilter1();
        updateFilter2();

        reset();
    }

    void DualFilter::prepare(double sampleRate, int blockSize)
    {
        juce::dsp::ProcessSpec spec;

        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = (juce::uint32)juce::jmax(0, blockSize);
        spec.numChannels = 2;

        prepare(spec);
    }

    void DualFilter::reset()
    {
        m_filter1.reset();
        m_filter2.reset();
    }

    void DualFilter::updateFilter1()
    {
        m_filter1.setMode(convertMode(m_mode1, m_slope1));
        m_filter1.setCutoffFrequencyHz(m_cutoffHz);
        m_filter1.setResonance(m_resonance);
    }

    void DualFilter::updateFilter2()
    {
        m_filter2.setMode(convertMode(m_mode2, m_slope2));
        m_filter2.setCutoffFrequencyHz(m_cutoffHz);
        m_filter2.setResonance(m_resonance);
    }

    void DualFilter::processSingleFilter(juce::dsp::AudioBlock<float>& block,
                                         juce::dsp::LadderFilter<float>& filter,
                                         FilterMode mode,
                                         FilterSlope slope)
    {
        if (mode != FilterMode::Notch)
        {
            filter.process(juce::dsp::ProcessContextReplacing<float>(block));
            return;
        }

        const auto numChannels = (int)block.getNumChannels();
        const auto numSamples = (int)block.getNumSamples();

        if (m_notchScratchBuffer.getNumChannels() < numChannels ||
            m_notchScratchBuffer.getNumSamples() < numSamples)
        {
            jassertfalse;
            return;
        }

        auto bandPassBlock = juce::dsp::AudioBlock<float>(m_notchScratchBuffer)
            .getSubsetChannelBlock(0, (size_t)numChannels)
            .getSubBlock(0, (size_t)numSamples);

        bandPassBlock.copyFrom(block);
        filter.setMode(convertMode(FilterMode::BandPass, slope));
        filter.process(juce::dsp::ProcessContextReplacing<float>(bandPassBlock));

        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* dry = block.getChannelPointer((size_t)ch);
            auto* bp = bandPassBlock.getChannelPointer((size_t)ch);
            for (int i = 0; i < numSamples; ++i)
                dry[i] -= (2.0f * bp[i]);
        }
    }

    void DualFilter::setCutoff(float cutoffHz)
    {
        float maxFreq = (float)(m_sampleRate * 0.49);

        m_cutoffHz = juce::jlimit(20.0f, maxFreq, cutoffHz);

        m_filter1.setCutoffFrequencyHz(m_cutoffHz);
        m_filter2.setCutoffFrequencyHz(m_cutoffHz);
    }

    void DualFilter::setResonance(float resonance)
    {
        m_resonance = juce::jlimit(0.0f, 1.0f, resonance);

        m_filter1.setResonance(m_resonance);
        m_filter2.setResonance(m_resonance);
    }

    void DualFilter::setModes(FilterMode mode1, FilterMode mode2, bool serial)
    {
        m_mode1 = mode1;
        m_mode2 = mode2;
        m_serialMode = serial;

        updateFilter1();
        updateFilter2();
    }

    void DualFilter::setMode1(FilterMode mode)
    {
        m_mode1 = mode;
        updateFilter1();
    }

    void DualFilter::setMode2(FilterMode mode)
    {
        m_mode2 = mode;
        updateFilter2();
    }

    void DualFilter::setSerialMode(bool serial)
    {
        m_serialMode = serial;
    }

    void DualFilter::setSlopes(FilterSlope slope1, FilterSlope slope2)
    {
        m_slope1 = slope1;
        m_slope2 = slope2;

        updateFilter1();
        updateFilter2();
    }

    void DualFilter::setSlope1(FilterSlope slope)
    {
        m_slope1 = slope;
        updateFilter1();
    }

    void DualFilter::setSlope2(FilterSlope slope)
    {
        m_slope2 = slope;
        updateFilter2();
    }

    void DualFilter::processBlock(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        processBlock(block);
    }

    void DualFilter::processBlock(juce::dsp::AudioBlock<float>& block)
    {
        if (!m_isPrepared)
        {
            jassertfalse;
            return;
        }

        if (block.getNumSamples() == 0)
            return;

        if (m_serialMode)
        {
            processSingleFilter(block, m_filter1, m_mode1, m_slope1);
            processSingleFilter(block, m_filter2, m_mode2, m_slope2);
        }
        else
        {
            auto numChannels = (int)block.getNumChannels();
            auto numSamples = (int)block.getNumSamples();

            // Real-time safety: never allocate in process.
            if (m_parallelBuffer.getNumChannels() < numChannels ||
                m_parallelBuffer.getNumSamples() < numSamples)
            {
                jassertfalse;
                return;
            }

            auto parallelBlock = juce::dsp::AudioBlock<float>(m_parallelBuffer)
                .getSubsetChannelBlock(0, (size_t)numChannels)
                .getSubBlock(0, (size_t)numSamples);

            parallelBlock.copyFrom(block);

            processSingleFilter(block, m_filter1, m_mode1, m_slope1);
            processSingleFilter(parallelBlock, m_filter2, m_mode2, m_slope2);

            block.add(parallelBlock);
            block.multiplyBy(0.5f);
        }
    }
}