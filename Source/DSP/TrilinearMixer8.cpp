#include "TrilinearMixer8.h"
#include "Utils/Math3D.h"
#include "Threading/AtomicGuiState.h"
#include <algorithm>

namespace DSP
{
    TrilinearMixer8::TrilinearMixer8(Threading::AtomicGuiState* guiState)
        : m_guiState(guiState)
    {
        m_gains = Utils::trilinearWeights(0.5f, 0.5f, 0.5f);
    }

    void TrilinearMixer8::prepare(double /*sampleRate*/)
    {
    }

    void TrilinearMixer8::updateGainsFromPosition(float x, float y, float z)
    {
        m_gains = Utils::trilinearWeights(x, y, z);
    }

    void TrilinearMixer8::updateGainsFromPosition(glm::vec3 position)
    {
        updateGainsFromPosition(position.x, position.y, position.z);
    }

    TrilinearMixer8::GainArray TrilinearMixer8::getCurrentGains() const
    {
        return m_gains;
    }

    void TrilinearMixer8::processBlock(const float* const* inputPointers, float* const* outputPointers,
                                       int numChannels, int numSamples)
    {
        if (inputPointers == nullptr || outputPointers == nullptr || numSamples <= 0)
            return;

        if (m_guiState != nullptr)
        {
            const glm::vec3 pos = m_guiState->getCursorPosition();
            m_gains = Utils::trilinearWeights(pos.x, pos.y, pos.z);
        }

        for (int s = 0; s < numSamples; ++s)
        {
            float sum = 0.0f;
            for (int i = 0; i < 8; ++i)
            {
                if (inputPointers[i] != nullptr)
                    sum += m_gains[static_cast<size_t>(i)] * inputPointers[i][s];
            }
            outputPointers[0][s] = sum;
            if (numChannels >= 2 && outputPointers[1] != nullptr)
                outputPointers[1][s] = sum;
        }
    }

    void TrilinearMixer8::processBlock(juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& outputBuffer,
                                      int startSample, int numSamples)
    {
        if (numSamples <= 0)
            return;
        if (inputBuffer.getNumChannels() < 8 || outputBuffer.getNumChannels() < 1)
            return;
        const int inputNumSamples = inputBuffer.getNumSamples();
        const int outputNumSamples = outputBuffer.getNumSamples();
        if (startSample < 0 || startSample + numSamples > inputNumSamples || startSample + numSamples > outputNumSamples)
            return;

        if (m_guiState != nullptr)
        {
            const glm::vec3 pos = m_guiState->getCursorPosition();
            m_gains = Utils::trilinearWeights(pos.x, pos.y, pos.z);
        }

        const int outChans = std::min(outputBuffer.getNumChannels(), 2);
        for (int s = 0; s < numSamples; ++s)
        {
            const int idx = startSample + s;
            float sum = 0.0f;
            for (int i = 0; i < 8; ++i)
                sum += m_gains[static_cast<size_t>(i)] * inputBuffer.getSample(i, idx);
            outputBuffer.setSample(0, idx, sum);
            if (outChans >= 2)
                outputBuffer.setSample(1, idx, sum);
        }
    }
}
