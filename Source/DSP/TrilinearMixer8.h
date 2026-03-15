#pragma once

#include <array>
#include <glm/glm.hpp>
#include <juce_dsp/juce_dsp.h>

namespace Threading
{
    class AtomicGuiState;
}

namespace DSP
{
    /**
     * 8-to-1 mixer using trilinear interpolation from a 3D position in the unit cube.
     * Oscillator index i corresponds to corner i in the same order as Utils::trilinearWeights()
     * (see Source/Utils/Math3D.h): 0:(0,0,0), 1:(1,0,0), 2:(0,1,0), 3:(1,1,0),
     * 4:(0,0,1), 5:(1,0,1), 6:(0,1,1), 7:(1,1,1).
     *
     * Pointer-based processBlock: 8 mono inputs, numChannels output (1 = mono, 2 = stereo with
     * same mix to L and R). Buffer-based processBlock: input channels 0..7 = oscillators 0..7;
     * output can be 1 or 2 channels.
     */
    class TrilinearMixer8
    {
    public:
        using GainArray = std::array<float, 8>;

        explicit TrilinearMixer8(Threading::AtomicGuiState* guiState);
        ~TrilinearMixer8() = default;

        /** Prepare for processing. Called once when sample rate is known. */
        void prepare(double sampleRate);

        /**
         * Process a block: mix 8 input channels into output using current gains.
         * Gains are updated from AtomicGuiState (trilinear weights) before mixing.
         * inputPointers[0..7] = 8 mono inputs; outputPointers[0], [1] = L, R if numChannels == 2.
         */
        void processBlock(const float* const* inputPointers, float* const* outputPointers,
                         int numChannels, int numSamples);

        /**
         * Process from buffers: channel i = oscillator i. outputBuffer can be mono or stereo.
         */
        void processBlock(juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& outputBuffer,
                         int startSample, int numSamples);

        /** Update gains from a 3D position in [0,1]^3 (e.g. for testing). Uses Utils::trilinearWeights. */
        void updateGainsFromPosition(float x, float y, float z);
        void updateGainsFromPosition(glm::vec3 position);

        /** Get current gains (for visualization or debugging). */
        GainArray getCurrentGains() const;

    private:
        std::array<float, 8> m_gains;
        Threading::AtomicGuiState* m_guiState;
    };
}
