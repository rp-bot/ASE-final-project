#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class LabeledPanel : public juce::Component
{
public:
    explicit LabeledPanel (juce::String text = {});

    void setText (const juce::String& text);
    void setFilledBackground (bool shouldFill) { filledBackground = shouldFill; repaint(); }

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Label titleLabel;
    bool filledBackground { false };
};
} // namespace UI
