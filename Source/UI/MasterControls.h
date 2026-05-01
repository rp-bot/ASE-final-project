#pragma once

#include <array>
#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "EnvelopeGraphEditor.h"
#include "FilterResponseEditor.h"

namespace UI {

class MasterControls: public juce::Component,
                      private juce::Timer {

public:

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    MasterControls(juce::AudioProcessorValueTreeState& apvts);
    ~MasterControls() override;

    void paint (juce::Graphics& g) override;
    void resized() override;
    void setSpectrumDataSource (const std::array<std::atomic<float>, 128>* bins,
                                const std::atomic<float>* sampleRateHz);
    void timerCallback() override;

private:
    class DragValueEditor : public juce::TextEditor
    {
    public:
        void configure (float minValue, float maxValue,
                        std::function<float()> valueGetter,
                        std::function<void(float)> valueSetter,
                        bool useLogDrag = false);

        void mouseDown (const juce::MouseEvent& event) override;
        void mouseDrag (const juce::MouseEvent& event) override;
        void mouseUp (const juce::MouseEvent& event) override;
        bool isDraggingValue() const noexcept { return didDrag; }

    private:
        float minV { 0.0f };
        float maxV { 1.0f };
        float dragStartValue { 0.0f };
        int dragStartY { 0 };
        bool didDrag { false };
        bool logDrag { false };
        std::function<float()> getter;
        std::function<void(float)> setter;
    };


    // LookAndFeel override that suppresses the mouse-hover highlight on the
    // toggle buttons (click-down feedback is preserved).
    class NoHoverButtonLookAndFeel : public juce::LookAndFeel_V4
    {
    public:
        void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                   const juce::Colour& backgroundColour,
                                   bool /*shouldDrawButtonAsHighlighted*/,
                                   bool shouldDrawButtonAsDown) override
        {
            juce::LookAndFeel_V4::drawButtonBackground (g, button, backgroundColour,
                                                        false, shouldDrawButtonAsDown);
        }
    };

    juce::Colour accent {juce::Colours::white};

    static constexpr int filterParams = 4;
    static constexpr int ampParams = 6;
    static constexpr int oscComponents = 8;

    void layoutMasterControls(juce::Rectangle<int> area);
    void configureRotarySlider(juce::Slider& slider, juce::Label& label, const juce::String& text);

    void propagateFilterMaster (int paramIndex);
    void propagateAmpMaster (int paramIndex);
    void seedCornerFromMaster (int corner, bool isFilter);
    static void writeNormalised (juce::AudioProcessorValueTreeState& apvts, const juce::String& parameterId,float realValue);
    void syncEditorsFromParameters();
    void commitFilterValueFromEditor (int index);
    void commitAmpValueFromEditor (int index);
    void configureValueEditor (DragValueEditor& editor, bool isFilter, int index);
    static juce::String formatParameterValue (const juce::String& name, float value);

    juce::AudioProcessorValueTreeState* apvtsPtr { nullptr };

    // juce::String getFilterParameterId (int paramIndex) const;
    // juce::String getAmpParameterId (int paramIndex) const;

    // text labels
    juce::Label filterControlTitle, ampControlTitle;
    std::array<juce::Label, filterParams> filterLabels;
    std::array<juce::Label, ampParams> ampLabels;

    // sliders
    std::array<juce::Slider, filterParams> filterSliders;
    std::array<juce::Slider, ampParams> ampSliders;
    std::array<DragValueEditor, filterParams> filterValueEditors;
    std::array<DragValueEditor, ampParams> ampValueEditors;

    // visualizers
    std::unique_ptr<FilterResponseEditor> filterEditor;
    std::unique_ptr<EnvelopeGraphEditor> envelopeEditor;

    // toggle buttons (TextButton in togglesState mode; gives us a coloured
    // "lit when on" look that honours buttonColourId / buttonOnColourId)
    std::array<juce::TextButton, oscComponents> filterToggles;
    std::array<juce::TextButton, oscComponents> ampToggles;
    NoHoverButtonLookAndFeel toggleLookAndFeel;

    // apvt attachment (for param update)
    std::array<std::unique_ptr<SliderAttachment>, filterParams> filterParamsAttachments;
    std::array<std::unique_ptr<ButtonAttachment>, oscComponents> filterToggleAttachments;

    std::array<std::unique_ptr<SliderAttachment>, ampParams> ampAttachments;
    std::array<std::unique_ptr<ButtonAttachment>, oscComponents> ampToggleAttachments;


};

}