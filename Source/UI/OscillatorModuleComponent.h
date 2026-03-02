#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class OscillatorModuleComponent : public juce::Component
{
public:
    static constexpr int paramsPerModule = 6;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    OscillatorModuleComponent (juce::AudioProcessorValueTreeState& apvts,
                               int cornerIndex,
                               const juce::String& titleText,
                               juce::Colour accentColour);
    ~OscillatorModuleComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    class AccentComboLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void setAccent (juce::Colour newAccent) { accent = newAccent; }

        void drawComboBox (juce::Graphics& g,
                           int width,
                           int height,
                           bool /*isButtonDown*/,
                           int /*buttonX*/,
                           int /*buttonY*/,
                           int /*buttonW*/,
                           int /*buttonH*/,
                           juce::ComboBox& /*box*/) override;

        juce::Font getComboBoxFont (juce::ComboBox& box) override;
        void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override;

    private:
        juce::Colour accent { juce::Colours::white };
    };

    class WaveformPreviewComponent : public juce::Component
    {
    public:
        void setAccentColour (juce::Colour colour) { accent = colour; repaint(); }
        void setWaveformIndex (int index) { waveformIndex = juce::jlimit (0, 3, index); repaint(); }
        void paint (juce::Graphics& g) override;

    private:
        int waveformIndex { 0 };
        juce::Colour accent { juce::Colours::white };
    };

    juce::String getCornerParameterId (int paramIndex) const;
    static void configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void updateWaveformPreviewFromSelector();

    int corner { 0 };
    juce::Colour accent;
    AccentComboLookAndFeel comboLookAndFeel;
    juce::Label titleLabel;
    juce::ComboBox waveformSelector;
    WaveformPreviewComponent waveformPreview;
    std::array<juce::Slider, paramsPerModule> paramSliders;
    std::array<juce::Label, paramsPerModule> paramLabels;
    std::array<std::unique_ptr<SliderAttachment>, paramsPerModule> paramAttachments;
    std::unique_ptr<ComboBoxAttachment> waveformAttachment;
};
} // namespace UI
