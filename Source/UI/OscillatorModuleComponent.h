#pragma once

#include <array>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{
class OscillatorModuleComponent : public juce::Component
{
public:
    static constexpr int synthParams = 6;
    static constexpr int filterParams = 4;
    static constexpr int ampParams = 6;

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
    enum class Page : int
    {
        Synth = 0,
        Filter = 1,
        Amp = 2
    };

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

    juce::String getSynthParameterId (int paramIndex) const;
    juce::String getFilterParameterId (int paramIndex) const;
    juce::String getAmpParameterId (int paramIndex) const;

    static void configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text);
    void updateWaveformPreviewFromSelector();
    void setPage (Page page);
    void refreshTabColours();
    void layoutSynthPage (juce::Rectangle<int> area);
    void layoutFilterPage (juce::Rectangle<int> area);
    void layoutAmpPage (juce::Rectangle<int> area);

    int corner { 0 };
    juce::Colour accent;
    Page activePage { Page::Synth };

    AccentComboLookAndFeel comboLookAndFeel;
    juce::Label titleLabel;
    juce::TextButton tabSynth { "Synth" };
    juce::TextButton tabFilter { "Filter" };
    juce::TextButton tabAmp { "Amp" };

    juce::ComboBox waveformSelector;
    WaveformPreviewComponent waveformPreview;

    std::array<juce::Slider, synthParams> synthSliders;
    std::array<juce::Label, synthParams> synthLabels;
    std::array<std::unique_ptr<SliderAttachment>, synthParams> synthAttachments;

    std::array<juce::Slider, filterParams> filterSliders;
    std::array<juce::Label, filterParams> filterLabels;
    std::array<std::unique_ptr<SliderAttachment>, filterParams> filterAttachments;

    std::array<juce::Slider, ampParams> ampSliders;
    std::array<juce::Label, ampParams> ampLabels;
    std::array<std::unique_ptr<SliderAttachment>, ampParams> ampAttachments;

    std::unique_ptr<ComboBoxAttachment> waveformAttachment;
};
} // namespace UI
