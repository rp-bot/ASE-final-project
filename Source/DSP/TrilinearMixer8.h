#pragma once

#include <array>
#include <glm/glm.hpp>
#include <juce_dsp/juce_dsp.h>

namespace DSP
{
    /**
     * 8-to-2 mixer using trilinear interpolation from a 3D position in the unit cube.
     * Per-oscillator pan (constant-power, -1 = full L, +1 = full R) is applied before weighting.
     */
    class TrilinearMixer8
    {
    public:
        using GainArray = std::array<float, 8>;

        TrilinearMixer8();
        ~TrilinearMixer8() = default;

        void prepare (double sampleRate);

        void processBlock (const float* const* inputPointers, float* const* outputPointers,
                           int numChannels, int numSamples, glm::vec3 cursor, const GainArray& pan);

        void processBlock (juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& outputBuffer,
                           int startSample, int numSamples, glm::vec3 cursor, const GainArray& pan);

        void updateGainsFromPosition (float x, float y, float z);
        void updateGainsFromPosition (glm::vec3 position);

        GainArray getCurrentGains() const;

    private:
        std::array<float, 8> m_gains {};
    };
}
