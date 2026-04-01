#include "TrilinearMixer8.h"
#include "Utils/Math3D.h"

namespace DSP
{
    TrilinearMixer8::TrilinearMixer8()
    {
        m_gains = Utils::trilinearWeights (0.5f, 0.5f, 0.5f);
    }

    void TrilinearMixer8::prepare (double /*sampleRate*/)
    {
    }

    void TrilinearMixer8::updateGainsFromPosition (float x, float y, float z)
    {
        m_gains = Utils::trilinearWeights (x, y, z);
    }

    void TrilinearMixer8::updateGainsFromPosition (glm::vec3 position)
    {
        updateGainsFromPosition (position.x, position.y, position.z);
    }

    TrilinearMixer8::GainArray TrilinearMixer8::getCurrentGains() const
    {
        return m_gains;
    }

    namespace
    {
        /** Pan in [-1, 1]: full L at -1, full R at +1, both unity at 0 (duplicate-mono style). */
        void stereoPanGains (float pan, float& outL, float& outR) noexcept
        {
            const float p = juce::jlimit (-1.0f, 1.0f, pan);
            outL = (p <= 0.0f) ? 1.0f : (1.0f - p);
            outR = (p >= 0.0f) ? 1.0f : (1.0f + p);
        }
    } // namespace

    void TrilinearMixer8::processBlock (const float* const* inputPointers, float* const* outputPointers,
                                        int numChannels, int numSamples, glm::vec3 cursor, const GainArray& pan)
    {
        if (inputPointers == nullptr || outputPointers == nullptr || numSamples <= 0)
            return;

        const glm::vec3 pos = Utils::clampToUnitCube (cursor);
        m_gains = Utils::trilinearWeights (pos);

        for (int s = 0; s < numSamples; ++s)
        {
            float sumL = 0.0f;
            float sumR = 0.0f;
            for (int i = 0; i < 8; ++i)
            {
                if (inputPointers[i] == nullptr)
                    continue;
                float gL = 1.0f;
                float gR = 1.0f;
                stereoPanGains (pan[static_cast<size_t> (i)], gL, gR);
                const float w = m_gains[static_cast<size_t> (i)] * inputPointers[i][s];
                sumL += w * gL;
                sumR += w * gR;
            }
            outputPointers[0][s] = sumL;
            if (numChannels >= 2 && outputPointers[1] != nullptr)
                outputPointers[1][s] = sumR;
        }
    }

    void TrilinearMixer8::processBlock (juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& outputBuffer,
                                        int startSample, int numSamples, glm::vec3 cursor, const GainArray& pan)
    {
        if (numSamples <= 0)
            return;
        if (inputBuffer.getNumChannels() < 8 || outputBuffer.getNumChannels() < 1)
            return;
        const int inputNumSamples = inputBuffer.getNumSamples();
        const int outputNumSamples = outputBuffer.getNumSamples();
        if (startSample < 0 || startSample + numSamples > inputNumSamples || startSample + numSamples > outputNumSamples)
            return;

        const glm::vec3 pos = Utils::clampToUnitCube (cursor);
        m_gains = Utils::trilinearWeights (pos);

        const int outChans = juce::jmin (outputBuffer.getNumChannels(), 2);
        for (int s = 0; s < numSamples; ++s)
        {
            const int idx = startSample + s;
            float sumL = 0.0f;
            float sumR = 0.0f;
            for (int i = 0; i < 8; ++i)
            {
                float gL = 1.0f;
                float gR = 1.0f;
                stereoPanGains (pan[static_cast<size_t> (i)], gL, gR);
                const float w = m_gains[static_cast<size_t> (i)] * inputBuffer.getSample (i, idx);
                sumL += w * gL;
                sumR += w * gR;
            }
            outputBuffer.setSample (0, idx, sumL);
            if (outChans >= 2)
                outputBuffer.setSample (1, idx, sumR);
        }
    }
}
