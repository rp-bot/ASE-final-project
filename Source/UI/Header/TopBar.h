#pragma once

#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "OutputSection.h"

namespace UI
{

// TODO(visage): swap LogoPlaceholder / title styling for branded assets.

/** Serum-style header: logo (left), title (center), master output (right). */
class LogoPlaceholder : public juce::Component
{
public:
    void paint (juce::Graphics& g) override;
};

class TopBar : public juce::Component
{
public:
    TopBar (juce::AudioProcessorValueTreeState& apvts,
            std::function<void()> onResetEngineHardOff);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setMeterLevels (float leftDb, float rightDb);
    OutputSection& getOutputSection() noexcept { return outputSection; }

private:
    LogoPlaceholder logoPlaceholder;
    juce::Label titleLabel;
    OutputSection outputSection;

    static constexpr int kLogoSide = 48;
    static constexpr int kOutputMinWidth = 280;
};

} // namespace UI
