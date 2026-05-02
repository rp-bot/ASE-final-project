#pragma once

#include <functional>
#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{

/** Rotary knob with a tight name label above and a clickable value label below (Ableton-style). */
class LabelledKnob : public juce::Component,
                     private juce::TextEditor::Listener,
                     private juce::Slider::Listener
{
public:
    LabelledKnob();
    ~LabelledKnob() override;

    juce::Slider& getSlider() noexcept { return rotary_; }
    juce::Label& getNameLabel() noexcept { return nameLabel_; }
    juce::Label& getValueLabel() noexcept { return valueLabel_; }

    void setNameLabelText (const juce::String& text);

    /** When null, display uses `Slider::getTextFromValue`. */
    void setValueFormatter (std::function<juce::String (double)> formatter);

    /** Characters allowed after the numeric core while editing (e.g. `" %"` for percent display). */
    void setValueEditExtraChars (juce::String chars);
    const juce::String& getValueEditExtraChars() const noexcept { return valueEditExtraChars_; }
    void refreshValueText();

    void resized() override;

private:
    friend struct ValueLabelMouseForwarder;

    struct ValueLabelMouseForwarder : public juce::MouseListener
    {
        LabelledKnob& owner;
        explicit ValueLabelMouseForwarder (LabelledKnob& o) : owner (o) {}
        void mouseDown (const juce::MouseEvent&) override { owner.showValueEditor(); }
    };

    void textEditorReturnKeyPressed (juce::TextEditor&) override;
    void textEditorEscapeKeyPressed (juce::TextEditor&) override;
    void textEditorFocusLost (juce::TextEditor&) override;
    void textEditorTextChanged (juce::TextEditor&) override;
    void sliderValueChanged (juce::Slider*) override;

    void updateValueLabel();
    void centerEditorText (juce::TextEditor& editor);
    void showValueEditor();
    void hideValueEditor (bool commitChanges);

    juce::Label nameLabel_;
    juce::Slider rotary_;
    juce::Label valueLabel_;
    ValueLabelMouseForwarder valueLabelMouse_ { *this };

    std::unique_ptr<juce::TextEditor> valueEditor_;
    std::function<juce::String (double)> valueFormatter_;

    juce::String valueEditExtraChars_;
};

} // namespace UI
