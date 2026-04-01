/*
  ==============================================================================

    TrilinearMixer8Test.cpp
    Unit tests for DSP::TrilinearMixer8 (8-to-2 volumetric mixer, trilinear gains + pan).

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DSP/TrilinearMixer8.h"
#include "Utils/Math3D.h"

namespace
{
constexpr float tol = 1e-5f;
constexpr int blockSize = 64;
}

class TrilinearMixer8Test : public juce::UnitTest
{
public:
    TrilinearMixer8Test() : juce::UnitTest ("TrilinearMixer8", "DSP") {}

    void runTest() override
    {
        runFixedPositionCorner000();
        runCenterPosition();
        runSumOfGains();
        runProcessBlockOneNonZeroInput();
        runProcessBlockBufferOverload();
    }

private:
    static DSP::TrilinearMixer8::GainArray zeroPan()
    {
        DSP::TrilinearMixer8::GainArray p {};
        return p;
    }

    void runFixedPositionCorner000()
    {
        beginTest ("Fixed position (0,0,0): one gain 1.0, others 0");

        DSP::TrilinearMixer8 mixer;
        mixer.updateGainsFromPosition (0.f, 0.f, 0.f);

        auto gains = mixer.getCurrentGains();
        expectWithinAbsoluteError (gains[0], 1.f, tol);
        for (size_t i = 1; i < 8; ++i)
            expectWithinAbsoluteError (gains[i], 0.f, tol);
    }

    void runCenterPosition()
    {
        beginTest ("Center (0.5, 0.5, 0.5): all gains 1/8");

        DSP::TrilinearMixer8 mixer;
        mixer.updateGainsFromPosition (0.5f, 0.5f, 0.5f);

        const float eighth = 1.f / 8.f;
        auto gains = mixer.getCurrentGains();
        for (size_t i = 0; i < 8; ++i)
            expectWithinAbsoluteError (gains[i], eighth, tol);
    }

    void runSumOfGains()
    {
        beginTest ("Sum of gains is 1.0 after update from any position");

        DSP::TrilinearMixer8 mixer;
        float pts[][3] = { { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f }, { 0.5f, 0.5f, 0.5f },
                           { 0.2f, 0.7f, 0.3f }, { 0.99f, 0.01f, 0.5f } };
        for (const auto& xyz : pts)
        {
            mixer.updateGainsFromPosition (xyz[0], xyz[1], xyz[2]);
            auto gains = mixer.getCurrentGains();
            float sum = 0.f;
            for (float g : gains)
                sum += g;
            expectWithinAbsoluteError (sum, 1.f, tol);
        }
    }

    void runProcessBlockOneNonZeroInput()
    {
        beginTest ("processBlock (pointers): position (0,0,0), pan 0, only input 0 non-zero -> output equals input 0");

        DSP::TrilinearMixer8 mixer;

        std::vector<float> in0 (blockSize, 0.5f);
        std::vector<float> in1 (blockSize, 0.f);
        std::vector<float> in2 (blockSize, 0.f);
        std::vector<float> in3 (blockSize, 0.f);
        std::vector<float> in4 (blockSize, 0.f);
        std::vector<float> in5 (blockSize, 0.f);
        std::vector<float> in6 (blockSize, 0.f);
        std::vector<float> in7 (blockSize, 0.f);
        const float* inputs[8] = { in0.data(), in1.data(), in2.data(), in3.data(),
                                   in4.data(), in5.data(), in6.data(), in7.data() };
        std::vector<float> outL (blockSize, 0.f);
        std::vector<float> outR (blockSize, 0.f);
        float* outputs[2] = { outL.data(), outR.data() };

        const glm::vec3 cursor (0.f, 0.f, 0.f);
        mixer.processBlock (inputs, outputs, 2, blockSize, cursor, zeroPan());

        for (int s = 0; s < blockSize; ++s)
        {
            expectWithinAbsoluteError (outL[static_cast<size_t> (s)], 0.5f, tol);
            expectWithinAbsoluteError (outR[static_cast<size_t> (s)], 0.5f, tol);
        }
    }

    void runProcessBlockBufferOverload()
    {
        beginTest ("processBlock (buffer): center position, pan 0, 8 equal inputs -> output equals input");

        DSP::TrilinearMixer8 mixer;

        juce::AudioBuffer<float> inputBuf (8, blockSize);
        juce::AudioBuffer<float> outputBuf (2, blockSize);
        for (int ch = 0; ch < 8; ++ch)
            for (int s = 0; s < blockSize; ++s)
                inputBuf.setSample (ch, s, 0.25f);
        outputBuf.clear();

        const glm::vec3 cursor (0.5f, 0.5f, 0.5f);
        mixer.processBlock (inputBuf, outputBuf, 0, blockSize, cursor, zeroPan());

        for (int s = 0; s < blockSize; ++s)
        {
            expectWithinAbsoluteError (outputBuf.getSample (0, s), 0.25f, tol);
            expectWithinAbsoluteError (outputBuf.getSample (1, s), 0.25f, tol);
        }
    }
};

static TrilinearMixer8Test trilinearMixer8Test;
