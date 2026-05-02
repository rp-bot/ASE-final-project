#include "LabelledKnob.h"

#include <cmath>

namespace UI
{
namespace
{
bool sliderAllowsDecimalPoint (const juce::Slider& s)
{
    const double iv = std::abs (s.getInterval());
    if (iv <= 0.0)
        return true;

    const double rounded = std::round (iv);
    return std::abs (iv - rounded) > 1e-12 || iv < 1.0;
}

bool sliderAllowsNegative (const juce::Slider& s)
{
    return s.getMinimum() < -1e-12;
}

bool isPartialNumericCore (const juce::String& core, bool negOk, bool decOk)
{
    if (core.isEmpty())
        return true;

    int i = 0;
    if (negOk && core[i] == '-')
        ++i;

    bool sawDot = false;
    for (; i < core.length(); ++i)
    {
        const auto c = core[i];
        if (c >= '0' && c <= '9')
            continue;

        if (decOk && c == '.' && ! sawDot)
        {
            sawDot = true;
            continue;
        }

        return false;
    }

    return true;
}

int lengthNumericCorePrefix (const juce::String& s, bool negOk, bool decOk)
{
    int i = 0;

    if (negOk && i < s.length() && s[i] == '-')
        ++i;

    bool sawDot = false;

    for (; i < s.length(); ++i)
    {
        const auto c = s[i];

        if (c >= '0' && c <= '9')
            continue;

        if (decOk && c == '.' && ! sawDot)
        {
            sawDot = true;
            continue;
        }

        break;
    }

    return i;
}

bool tailMatchesEditableSuffix (const juce::String& tail, const juce::Slider& slider, const juce::String& extraChars)
{
    if (tail.isEmpty())
        return true;

    const juce::String pattern = slider.getTextValueSuffix() + extraChars;

    if (pattern.isNotEmpty() && pattern.startsWithIgnoreCase (tail))
        return true;

    if (extraChars.containsChar ('%'))
    {
        const auto t = tail.trimStart();

        if (t.startsWithChar ('%'))
            return t.length() == 1;
    }

    return false;
}

juce::String filterKnobNumericInsertion (LabelledKnob& knob, juce::TextEditor& ed, const juce::String& newInput)
{
    auto& slider = knob.getSlider();
    const bool negOk = sliderAllowsNegative (slider);
    const bool decOk = sliderAllowsDecimalPoint (slider);
    const auto& extras = knob.getValueEditExtraChars();

    juce::String allowed = "0123456789";

    if (negOk)
        allowed += "-";

    if (decOk)
        allowed += ".";

    allowed += extras;

    const auto sfx = slider.getTextValueSuffix();

    for (int i = 0; i < sfx.length(); ++i)
    {
        const auto c = sfx[i];

        if (! allowed.containsChar (c))
            allowed += juce::String::charToString (c);
    }

    const juce::String sanitized = newInput.retainCharacters (allowed);

    const juce::Range<int> sel = ed.getHighlightedRegion();
    const juce::String before = ed.getText().substring (0, sel.getStart());
    const juce::String after = ed.getText().substring (sel.getEnd());

    juce::String out;

    for (int i = 0; i < sanitized.length(); ++i)
    {
        const juce::String chunk = sanitized.substring (i, i + 1);
        const juce::String trial = before + out + chunk + after;

        const int coreLen = lengthNumericCorePrefix (trial, negOk, decOk);
        const juce::String core = trial.substring (0, coreLen);
        const juce::String tail = trial.substring (coreLen);

        if (! isPartialNumericCore (core, negOk, decOk))
            continue;

        if (! tailMatchesEditableSuffix (tail, slider, extras))
            continue;

        out += chunk;
    }

    return out;
}

struct KnobValueInputFilter final : public juce::TextEditor::InputFilter
{
    LabelledKnob* knob { nullptr };

    explicit KnobValueInputFilter (LabelledKnob& k) : knob (&k) {}

    juce::String filterNewText (juce::TextEditor& ed, const juce::String& newInput) override
    {
        return filterKnobNumericInsertion (*knob, ed, newInput);
    }
};

} // namespace

LabelledKnob::LabelledKnob()
{
    nameLabel_.setJustificationType (juce::Justification::centred);
    nameLabel_.setFont (juce::Font (11.0f));
    nameLabel_.setBorderSize ({});
    nameLabel_.setInterceptsMouseClicks (false, false);

    rotary_.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    rotary_.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

    valueLabel_.setJustificationType (juce::Justification::centred);
    valueLabel_.setFont (juce::Font (11.0f));
    valueLabel_.setBorderSize ({});
    valueLabel_.setInterceptsMouseClicks (true, false);

    addAndMakeVisible (nameLabel_);
    addAndMakeVisible (rotary_);
    addAndMakeVisible (valueLabel_);

    valueLabel_.addMouseListener (&valueLabelMouse_, false);
    rotary_.addListener (this);

    updateValueLabel();
}

LabelledKnob::~LabelledKnob()
{
    valueLabel_.removeMouseListener (&valueLabelMouse_);
    rotary_.removeListener (this);

    if (valueEditor_ != nullptr)
    {
        valueEditor_->removeListener (this);
        removeChildComponent (valueEditor_.get());
        valueEditor_.reset();
    }
}

void LabelledKnob::setNameLabelText (const juce::String& text)
{
    nameLabel_.setText (text, juce::dontSendNotification);
}

void LabelledKnob::setValueFormatter (std::function<juce::String (double)> formatter)
{
    valueFormatter_ = std::move (formatter);
    updateValueLabel();
}

void LabelledKnob::setValueEditExtraChars (juce::String chars)
{
    valueEditExtraChars_ = std::move (chars);
}

void LabelledKnob::refreshValueText()
{
    updateValueLabel();
}

void LabelledKnob::resized()
{
    auto r = getLocalBounds();

    // No extra spacing: labels sit directly against the rotary knob.
    const int nameH = juce::roundToInt (std::ceil ((double) nameLabel_.getFont().getHeight()));
    const int valueH = juce::roundToInt (std::ceil ((double) valueLabel_.getFont().getHeight()));

    nameLabel_.setBounds (r.removeFromTop (nameH));
    valueLabel_.setBounds (r.removeFromBottom (valueH));

    // JUCE rotary rendering keeps some internal inset; expand vertically to remove visible gaps.
    constexpr int knobVerticalBleed = 4;
    rotary_.setBounds (r.expanded (0, knobVerticalBleed));
}

void LabelledKnob::updateValueLabel()
{
    const double v = rotary_.getValue();
    const juce::String text = valueFormatter_ != nullptr ? valueFormatter_ (v) : rotary_.getTextFromValue (v);
    valueLabel_.setText (text, juce::dontSendNotification);
}

void LabelledKnob::centerEditorText (juce::TextEditor& editor)
{
    const auto textWidth = juce::roundToInt (editor.getFont().getStringWidthFloat (editor.getText()));
    const int leftIndent = juce::jmax (2, (editor.getWidth() - textWidth) / 2);
    editor.setIndents (leftIndent, 1);
}

void LabelledKnob::showValueEditor()
{
    if (valueEditor_ != nullptr)
        return;

    valueEditor_ = std::make_unique<juce::TextEditor>();
    valueEditor_->setBounds (valueLabel_.getBounds());
    valueEditor_->setMultiLine (false);
    valueEditor_->setReturnKeyStartsNewLine (false);
    valueEditor_->setScrollbarsShown (false);
    valueEditor_->setCaretVisible (true);
    valueEditor_->setPopupMenuEnabled (false);
    valueEditor_->setSelectAllWhenFocused (true);
    valueEditor_->setColour (juce::TextEditor::backgroundColourId, juce::Colours::black.withAlpha (0.9f));
    valueEditor_->setColour (juce::TextEditor::outlineColourId, juce::Colours::grey.withAlpha (0.8f));
    valueEditor_->setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::cyan.withAlpha (0.9f));
    valueEditor_->setColour (juce::TextEditor::textColourId, valueLabel_.findColour (juce::Label::textColourId));
    valueEditor_->setFont (valueLabel_.getFont());
    valueEditor_->setText (rotary_.getTextFromValue (rotary_.getValue()), juce::dontSendNotification);
    centerEditorText (*valueEditor_);
    valueEditor_->setInputFilter (new KnobValueInputFilter (*this), true);
    valueEditor_->addListener (this);

    valueLabel_.setVisible (false);
    addAndMakeVisible (*valueEditor_);
    valueEditor_->grabKeyboardFocus();
    valueEditor_->selectAll();
}

void LabelledKnob::hideValueEditor (bool commitChanges)
{
    if (valueEditor_ == nullptr)
        return;

    juce::String typed;

    if (commitChanges)
        typed = valueEditor_->getText();

    valueEditor_->removeListener (this);
    removeChildComponent (valueEditor_.get());
    valueEditor_.reset();
    valueLabel_.setVisible (true);

    if (commitChanges)
    {
        const double v = rotary_.getValueFromText (typed);
        rotary_.setValue (v, juce::sendNotificationSync);
    }

    updateValueLabel();
}

void LabelledKnob::textEditorReturnKeyPressed (juce::TextEditor& ed)
{
    if (valueEditor_.get() == &ed)
        hideValueEditor (true);
}

void LabelledKnob::textEditorEscapeKeyPressed (juce::TextEditor& ed)
{
    if (valueEditor_.get() == &ed)
        hideValueEditor (false);
}

void LabelledKnob::textEditorFocusLost (juce::TextEditor& ed)
{
    if (valueEditor_.get() == &ed)
        hideValueEditor (true);
}

void LabelledKnob::textEditorTextChanged (juce::TextEditor& ed)
{
    if (valueEditor_.get() == &ed)
        centerEditorText (ed);
}

void LabelledKnob::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &rotary_)
        updateValueLabel();
}

} // namespace UI
