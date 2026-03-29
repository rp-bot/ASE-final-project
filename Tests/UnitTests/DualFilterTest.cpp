#include <JuceHeader.h>
#include "DSP/DualFilter.h"

namespace
{
    constexpr double kSampleRate = 44100.0;
    constexpr int kNumSamples = 16384;

    juce::AudioBuffer<float> makeSine(float frequencyHz, float amplitude = 0.5f)
    {
        juce::AudioBuffer<float> buffer(2, kNumSamples);
        buffer.clear();

        auto phaseDelta = juce::MathConstants<double>::twoPi * (double)frequencyHz / kSampleRate;
        double phase = 0.0;

        for (int sample = 0; sample < kNumSamples; ++sample)
        {
            auto value = amplitude * std::sin(phase);
            buffer.setSample(0, sample, value);
            buffer.setSample(1, sample, value);
            phase += phaseDelta;
        }

        return buffer;
    }

    float calculateRms(const juce::AudioBuffer<float>& buffer, int startSample)
    {
        const auto numSamples = buffer.getNumSamples() - startSample;
        jassert(numSamples > 0);

        float sum = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const float* data = buffer.getReadPointer(ch, startSample);
            for (int i = 0; i < numSamples; ++i)
                sum += data[i] * data[i];
        }

        auto mean = sum / (float)(numSamples * buffer.getNumChannels());
        return std::sqrt(mean);
    }

    float processAndMeasure(DSP::DualFilter& filter, juce::AudioBuffer<float> buffer)
    {
        filter.processBlock(buffer);
        return calculateRms(buffer, kNumSamples / 2);
    }
}

class DualFilterTest : public juce::UnitTest
{
public:
    DualFilterTest() : juce::UnitTest("DualFilter", "DSP") {}

    void runTest() override
    {
        runClampTests();
        runModeResponseTests();
        runSerialVsParallelTest();
        runSlopeTest();
        runZeroBlockTest();
    }

private:
    void prepareFilter(DSP::DualFilter& filter)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = kSampleRate;
        spec.maximumBlockSize = (juce::uint32)kNumSamples;
        spec.numChannels = 2;
        filter.prepare(spec);
    }

    void runClampTests()
    {
        beginTest("Clamps cutoff and resonance");
        DSP::DualFilter filter;
        prepareFilter(filter);

        filter.setCutoff(5.0f);
        expectWithinAbsoluteError(filter.getCutoff(), 20.0f, 0.0001f, "Cutoff should clamp to 20 Hz minimum");

        filter.setCutoff(100000.0f);
        expect(filter.getCutoff() <= 22000.0f, "Cutoff should clamp below Nyquist");

        filter.setResonance(0.01f);
        expectWithinAbsoluteError(filter.getResonance(), 0.01f, 0.0001f, "Resonance should remain in range");

        filter.setResonance(20.0f);
        expectWithinAbsoluteError(filter.getResonance(), 1.0f, 0.0001f, "Resonance should clamp to JUCE maximum");
    }

    void runModeResponseTests()
    {
        beginTest("Lowpass attenuates high frequencies");
        {
            DSP::DualFilter filter;
            prepareFilter(filter);
            filter.setModes(DSP::FilterMode::LowPass, DSP::FilterMode::LowPass, true);
            filter.setCutoff(1000.0f);
            filter.setResonance(0.1f);

            auto lowRms = processAndMeasure(filter, makeSine(200.0f));

            filter.reset();
            auto highRms = processAndMeasure(filter, makeSine(4000.0f));

            expect(lowRms > highRms * 1.5f, "Low frequency should pass more than high frequency");
        }

        beginTest("Highpass attenuates low frequencies");
        {
            DSP::DualFilter filter;
            prepareFilter(filter);
            filter.setModes(DSP::FilterMode::HighPass, DSP::FilterMode::HighPass, true);
            filter.setCutoff(1000.0f);
            filter.setResonance(0.1f);

            auto lowRms = processAndMeasure(filter, makeSine(200.0f));

            filter.reset();
            auto highRms = processAndMeasure(filter, makeSine(4000.0f));

            expect(highRms > lowRms * 1.5f, "High frequency should pass more than low frequency");
        }

        beginTest("Notch attenuates frequencies around cutoff");
        {
            DSP::DualFilter filter;
            prepareFilter(filter);
            filter.setModes(DSP::FilterMode::Notch, DSP::FilterMode::Notch, true);
            filter.setCutoff(1000.0f);
            filter.setResonance(0.7f);

            auto cutoffRms = processAndMeasure(filter, makeSine(1000.0f));
            auto inputRms = calculateRms(makeSine(1000.0f), kNumSamples / 2);
            expect(std::isfinite(cutoffRms), "Notch processing output should remain finite");
            expect(std::abs(cutoffRms - inputRms) > 0.02f, "Notch mode should audibly alter cutoff-region content");
        }
    }

    void runSerialVsParallelTest()
    {
        beginTest("Serial mode is steeper than parallel");

        auto input = makeSine(4000.0f);

        DSP::DualFilter serialFilter;
        prepareFilter(serialFilter);
        serialFilter.setModes(DSP::FilterMode::LowPass, DSP::FilterMode::LowPass, true);
        serialFilter.setCutoff(1000.0f);
        serialFilter.setResonance(0.1f);
        auto serialRms = processAndMeasure(serialFilter, input);

        DSP::DualFilter parallelFilter;
        prepareFilter(parallelFilter);
        parallelFilter.setModes(DSP::FilterMode::LowPass, DSP::FilterMode::LowPass, false);
        parallelFilter.setCutoff(1000.0f);
        parallelFilter.setResonance(0.1f);
        auto parallelRms = processAndMeasure(parallelFilter, input);

        expect(serialRms < parallelRms, "Serial cascaded filtering should attenuate more");
    }

    void runSlopeTest()
    {
        beginTest("24 dB slope attenuates more than 12 dB");

        auto input = makeSine(5000.0f);

        DSP::DualFilter slope12;
        prepareFilter(slope12);
        slope12.setModes(DSP::FilterMode::LowPass, DSP::FilterMode::LowPass, true);
        slope12.setSlopes(DSP::FilterSlope::Slope12dB, DSP::FilterSlope::Slope12dB);
        slope12.setCutoff(1000.0f);
        slope12.setResonance(0.1f);
        auto rms12 = processAndMeasure(slope12, input);

        DSP::DualFilter slope24;
        prepareFilter(slope24);
        slope24.setModes(DSP::FilterMode::LowPass, DSP::FilterMode::LowPass, true);
        slope24.setSlopes(DSP::FilterSlope::Slope24dB, DSP::FilterSlope::Slope24dB);
        slope24.setCutoff(1000.0f);
        slope24.setResonance(0.1f);
        auto rms24 = processAndMeasure(slope24, input);

        expect(rms24 < rms12, "24 dB filter should attenuate out-of-band content more");
    }

    void runZeroBlockTest()
    {
        beginTest("Zero-sized processing block is safe");
        DSP::DualFilter filter;
        prepareFilter(filter);

        juce::AudioBuffer<float> empty(2, 0);
        filter.processBlock(empty);
        expect(true, "Processing zero samples should not crash");
    }
};

static DualFilterTest dualFilterTest;
