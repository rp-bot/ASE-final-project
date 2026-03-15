#include "AmpEnvelope.h"
#include <cmath>
#include <algorithm>

namespace DSP {

AmpEnvelope::AmpEnvelope()
    : m_currentStage(EnvelopeStage::Idle)
    , m_currentLevel(0.0f)
    , m_attackTime(0.01f)
    , m_decayTime(0.1f)
    , m_sustainLevel(0.7f)
    , m_releaseTime(0.3f)
    , m_sampleRate(44100.0)
    , m_attackSamples(441)
    , m_decaySamples(4410)
    , m_releaseSamples(13230)
    , m_stageSampleCount(0)
    , m_releaseStartLevel(0.0f)
{}

void AmpEnvelope::prepare(double sampleRate) {
    m_sampleRate = sampleRate;
    recalculateSamples();
    reset();
}

void AmpEnvelope::recalculateSamples() {
    // minimum of 1 sample per stage to avoid division by zero
    m_attackSamples  = std::max(1, static_cast<int>(m_attackTime  * m_sampleRate));
    m_decaySamples   = std::max(1, static_cast<int>(m_decayTime   * m_sampleRate));
    m_releaseSamples = std::max(1, static_cast<int>(m_releaseTime * m_sampleRate));
}

float AmpEnvelope::processSample() {
    switch (m_currentStage) {
        case EnvelopeStage::Idle: {
            m_currentLevel = 0.0f;
            return 0.0f;
        }
        case EnvelopeStage::Attack: {
            m_stageSampleCount++;

            if (m_stageSampleCount >= m_attackSamples) {
                m_currentLevel     = 1.0f;
                m_currentStage     = EnvelopeStage::Decay;
                m_stageSampleCount = 0;
            }
            else {
                // starts fast, settles into 1.0 smoothly
                const float t    = static_cast<float>(m_stageSampleCount) / static_cast<float>(m_attackSamples);
                m_currentLevel   = 1.0f - std::exp(-t * 5.0f);
            }
            return m_currentLevel;
        }

        case EnvelopeStage::Decay: {
            m_stageSampleCount++;

            if (m_stageSampleCount >= m_decaySamples) {
                m_currentLevel = m_sustainLevel;
                m_currentStage = EnvelopeStage::Sustain;
                m_stageSampleCount = 0;
            }
            else {
                // Exponential fall from 1.0 down to sustain level
                const float t = static_cast<float>(m_stageSampleCount) / static_cast<float>(m_decaySamples);
                m_currentLevel = m_sustainLevel + (1.0f - m_sustainLevel) * std::exp(-t * 5.0f);
            }
            return m_currentLevel;
        }

        case EnvelopeStage::Sustain:
            m_currentLevel = m_sustainLevel;
            return m_sustainLevel;

        case EnvelopeStage::Release: {
            m_stageSampleCount++;

            if (m_stageSampleCount >= m_releaseSamples) {
                m_currentLevel = 0.0f;
                m_currentStage = EnvelopeStage::Idle;
                m_stageSampleCount = 0;
            }
            else {
                // Exponential fall from the level captured at note-off down to 0.0
                const float t  = static_cast<float>(m_stageSampleCount) / static_cast<float>(m_releaseSamples);
                m_currentLevel = m_releaseStartLevel * std::exp(-t * 5.0f);
            }
            return m_currentLevel;
        }
    }

    return m_currentLevel;
}

void AmpEnvelope::noteOn() {
    // always restart from Attack regardless of current stage

    m_currentStage = EnvelopeStage::Attack;
    m_stageSampleCount = 0;
    // Note: m_currentLevel is NOT reset here so that a legato retrigger
    // begins its exponential rise from the current amplitude, preventing clicks.
}

void AmpEnvelope::noteOff() {
    if (m_currentStage != EnvelopeStage::Idle) {
        // Capture the level at the moment of note-off so the release curve starts from exactly where we are now
        m_releaseStartLevel = m_currentLevel;
        m_currentStage = EnvelopeStage::Release;
        m_stageSampleCount  = 0;
    }
}

void AmpEnvelope::setADSR(float attackTime, float decayTime, float sustainLevel, float releaseTime) {
    setAttackTime(attackTime);
    setDecayTime(decayTime);
    setSustainLevel(sustainLevel);
    setReleaseTime(releaseTime);
}

void AmpEnvelope::setAttackTime(float timeSeconds) {
    m_attackTime    = std::max(0.0001f, timeSeconds);
    m_attackSamples = std::max(1, static_cast<int>(m_attackTime * m_sampleRate));
}

void AmpEnvelope::setDecayTime(float timeSeconds) {
    m_decayTime    = std::max(0.0001f, timeSeconds);
    m_decaySamples = std::max(1, static_cast<int>(m_decayTime * m_sampleRate));
}

void AmpEnvelope::setSustainLevel(float level) {
    m_sustainLevel = std::min(1.0f, std::max(0.0f, level));
}

void AmpEnvelope::setReleaseTime(float timeSeconds) {
    m_releaseTime    = std::max(0.0001f, timeSeconds);
    m_releaseSamples = std::max(1, static_cast<int>(m_releaseTime * m_sampleRate));
}

EnvelopeStage AmpEnvelope::getCurrentStage() const {
    return m_currentStage;
}

float AmpEnvelope::getCurrentLevel() const {
    return m_currentLevel;
}

void AmpEnvelope::reset() {
    m_currentStage      = EnvelopeStage::Idle;
    m_currentLevel      = 0.0f;
    m_stageSampleCount  = 0;
    m_releaseStartLevel = 0.0f;
}

bool AmpEnvelope::isActive() const {
    return m_currentStage != EnvelopeStage::Idle;
}

}