#include "SynthLookAndFeel.h"

namespace UI
{

SynthLookAndFeel::SynthLookAndFeel()
{
    // ── Global colour overrides ──────────────────────────────────────────────
    setColour (juce::ResizableWindow::backgroundColourId, background());

    // Labels
    setColour (juce::Label::textColourId,           textPrimary());
    setColour (juce::Label::textWhenEditingColourId, textPrimary());
    setColour (juce::Label::backgroundColourId,      juce::Colours::transparentBlack);
    setColour (juce::Label::outlineColourId,         juce::Colours::transparentBlack);

    // Sliders — default accent teal (modules will override with their accent)
    setColour (juce::Slider::thumbColourId,                teal());
    setColour (juce::Slider::rotarySliderFillColourId,     teal());
    setColour (juce::Slider::rotarySliderOutlineColourId,  arcTrack());
    setColour (juce::Slider::trackColourId,                arcTrack());
    setColour (juce::Slider::backgroundColourId,           arcTrack());

    // TextEditor (inline knob value entry)
    setColour (juce::TextEditor::backgroundColourId,      panelSurface());
    setColour (juce::TextEditor::textColourId,            textPrimary());
    setColour (juce::TextEditor::outlineColourId,         panelBorder());
    setColour (juce::TextEditor::focusedOutlineColourId,  teal());
    setColour (juce::TextEditor::highlightColourId,       teal().withAlpha (0.3f));
    setColour (juce::TextEditor::highlightedTextColourId, textPrimary());

    // Buttons
    setColour (juce::TextButton::buttonColourId,   panelSurface());
    setColour (juce::TextButton::buttonOnColourId,  teal().withAlpha (0.80f));
    setColour (juce::TextButton::textColourOffId,   textPrimary());
    setColour (juce::TextButton::textColourOnId,    juce::Colours::white);

    // ComboBox
    setColour (juce::ComboBox::backgroundColourId, panelSurface());
    setColour (juce::ComboBox::textColourId,        textPrimary());
    setColour (juce::ComboBox::outlineColourId,     panelBorder());
    setColour (juce::ComboBox::arrowColourId,       textDim());

    // PopupMenu
    setColour (juce::PopupMenu::backgroundColourId,            panelSurface());
    setColour (juce::PopupMenu::textColourId,                  textPrimary());
    setColour (juce::PopupMenu::highlightedBackgroundColourId, teal().withAlpha (0.15f));
    setColour (juce::PopupMenu::highlightedTextColourId,       textPrimary());

    // ScrollBar
    setColour (juce::ScrollBar::thumbColourId, panelBorder());

    // ── Typography ───────────────────────────────────────────────────────────
    setDefaultSansSerifTypefaceName ("Helvetica Neue");
}

// ────────────────────────────────────────────────────────────────────────────
// Rotary knob — flat disc, coloured arc
// ────────────────────────────────────────────────────────────────────────────
void SynthLookAndFeel::drawRotarySlider (juce::Graphics& g,
                                         int x, int y, int width, int height,
                                         float sliderPosProportional,
                                         float rotaryStartAngle,
                                         float rotaryEndAngle,
                                         juce::Slider& slider)
{
    const float cx     = static_cast<float> (x) + static_cast<float> (width)  * 0.5f;
    const float cy     = static_cast<float> (y) + static_cast<float> (height) * 0.5f;
    const float radius = (juce::jmin (width, height) * 0.5f) - 4.0f;

    if (radius <= 0.0f)
        return;

    const float arcThickness = juce::jmax (2.5f, radius * 0.14f);

    // ── Arc radii ────────────────────────────────────────────────────────────
    const float arcOuter = radius;
    const float arcInner = radius - arcThickness;

    // ── Value angle ──────────────────────────────────────────────────────────
    const float valueAngle = rotaryStartAngle +
                             sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // ── Track arc (background) ───────────────────────────────────────────────
    {
        juce::Path track;
        track.addCentredArc (cx, cy, (arcOuter + arcInner) * 0.5f, (arcOuter + arcInner) * 0.5f,
                             0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (arcTrack());
        g.strokePath (track, juce::PathStrokeType (arcThickness,
                                                   juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));
    }

    // ── Filled arc (value) ───────────────────────────────────────────────────
    {
        const juce::Colour fillColour =
            slider.findColour (juce::Slider::rotarySliderFillColourId);

        float fillStartAngle = rotaryStartAngle;
        float fillEndAngle = valueAngle;
        bool shouldDrawFill = true;

        const double minValue = slider.getMinimum();
        const double maxValue = slider.getMaximum();
        const bool crossesZero = minValue < 0.0 && maxValue > 0.0;
        const double range = maxValue - minValue;
        const bool zeroIsCentered = range > 0.0 && std::abs ((maxValue + minValue) / range) < 1.0e-6;

        if (crossesZero && zeroIsCentered)
        {
            const float zeroProportional = static_cast<float> (slider.valueToProportionOfLength (0.0));
            const float zeroAngle = rotaryStartAngle + zeroProportional * (rotaryEndAngle - rotaryStartAngle);
            fillStartAngle = juce::jmin (zeroAngle, valueAngle);
            fillEndAngle = juce::jmax (zeroAngle, valueAngle);

            if (std::abs (slider.getValue()) <= 1.0e-9)
                shouldDrawFill = false;
        }

        if (shouldDrawFill && std::abs (fillEndAngle - fillStartAngle) > 1.0e-6f)
        {
            juce::Path filled;
            filled.addCentredArc (cx, cy, (arcOuter + arcInner) * 0.5f, (arcOuter + arcInner) * 0.5f,
                                  0.0f, fillStartAngle, fillEndAngle, true);
            g.setColour (fillColour);
            g.strokePath (filled, juce::PathStrokeType (arcThickness,
                                                        juce::PathStrokeType::curved,
                                                        juce::PathStrokeType::rounded));
        }
    }

    // ── Disc body ────────────────────────────────────────────────────────────
    const float bodyRadius = arcInner - 3.0f;
    if (bodyRadius > 0.0f)
    {
        g.setColour (knobBody());
        g.fillEllipse (cx - bodyRadius, cy - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f);

        g.setColour (panelBorder());
        g.drawEllipse (cx - bodyRadius, cy - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f, 1.0f);
    }

// ── Pointer triangle ─────────────────────────────────────────────────────
    {
        const juce::Colour pointerColour =
            slider.findColour (juce::Slider::rotarySliderFillColourId);

        const float pointerLen  = juce::jmax (7.0f, bodyRadius * 0.34f);
        const float pointerHalf = juce::jmax (2.2f, bodyRadius * 0.10f);
        const float tipR        = bodyRadius - 2.0f;
        const float baseR       = tipR - pointerLen;

        const float ux = std::sin (valueAngle);
        const float uy = -std::cos (valueAngle);
        const float px = -uy;
        const float py = ux;

        const juce::Point<float> tip  { cx + tipR  * ux, cy + tipR  * uy };
        const juce::Point<float> base { cx + baseR * ux, cy + baseR * uy };
        const juce::Point<float> left { base.x + pointerHalf * px, base.y + pointerHalf * py };
        const juce::Point<float> right{ base.x - pointerHalf * px, base.y - pointerHalf * py };

        juce::Path pointer;
        pointer.startNewSubPath (tip);
        pointer.lineTo (left);
        pointer.lineTo (right);
        pointer.closeSubPath();

        g.setColour (pointerColour);
        g.fillPath (pointer);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Linear slider — thin rounded track, flat accent thumb
// ────────────────────────────────────────────────────────────────────────────
void SynthLookAndFeel::drawLinearSlider (juce::Graphics& g,
                                         int x, int y, int width, int height,
                                         float sliderPos,
                                         float /*minSliderPos*/,
                                         float /*maxSliderPos*/,
                                         juce::Slider::SliderStyle style,
                                         juce::Slider& slider)
{
    const float trackW = 3.0f;
    const float thumbR = 6.0f;
    const bool  isVert = (style == juce::Slider::LinearVertical);

    const juce::Colour accent = slider.findColour (juce::Slider::thumbColourId);

    if (isVert)
    {
        const float tx   = static_cast<float> (x) + static_cast<float> (width)  * 0.5f - trackW * 0.5f;
        const float ty   = static_cast<float> (y);
        const float th   = static_cast<float> (height);

        g.setColour (arcTrack());
        g.fillRoundedRectangle (tx, ty, trackW, th, trackW * 0.5f);

        g.setColour (accent);
        g.fillRoundedRectangle (tx, sliderPos - trackW * 0.5f, trackW,
                                (ty + th) - sliderPos + trackW * 0.5f, trackW * 0.5f);

        g.setColour (accent);
        g.fillEllipse (static_cast<float> (x) + static_cast<float> (width)  * 0.5f - thumbR,
                       sliderPos - thumbR,
                       thumbR * 2.0f, thumbR * 2.0f);
    }
    else
    {
        const float ty = static_cast<float> (y) + static_cast<float> (height) * 0.5f - trackW * 0.5f;
        const float tl = static_cast<float> (x);
        const float tw = static_cast<float> (width);

        g.setColour (arcTrack());
        g.fillRoundedRectangle (tl, ty, tw, trackW, trackW * 0.5f);

        g.setColour (accent);
        g.fillRoundedRectangle (tl, ty, sliderPos - tl, trackW, trackW * 0.5f);

        g.setColour (accent);
        g.fillEllipse (sliderPos - thumbR,
                       static_cast<float> (y) + static_cast<float> (height) * 0.5f - thumbR,
                       thumbR * 2.0f, thumbR * 2.0f);
    }
}

// ────────────────────────────────────────────────────────────────────────────
// Button background — rounded rect, teal when active
// ────────────────────────────────────────────────────────────────────────────
void SynthLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                              juce::Button& button,
                                              const juce::Colour& /*backgroundColour*/,
                                              bool isHighlighted,
                                              bool isDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    const float corner = 4.0f;

    const juce::Colour onColour = button.isColourSpecified (juce::TextButton::buttonOnColourId)
                                      ? button.findColour (juce::TextButton::buttonOnColourId)
                                      : teal().withAlpha (0.80f);

    const juce::Colour offColour = button.isColourSpecified (juce::TextButton::buttonColourId)
                                       ? button.findColour (juce::TextButton::buttonColourId)
                                       : panelSurface();

    juce::Colour fill;
    if (button.getToggleState())
        fill = onColour;
    else if (isDown)
        fill = panelBorder().withAlpha (0.9f);
    else if (isHighlighted)
        fill = offColour.darker (0.05f);
    else
        fill = offColour;

    g.setColour (fill);
    g.fillRoundedRectangle (bounds, corner);

    g.setColour (panelBorder());
    g.drawRoundedRectangle (bounds, corner, 1.0f);
}

// ────────────────────────────────────────────────────────────────────────────
// Label font — Helvetica Neue at the label's own size
// ────────────────────────────────────────────────────────────────────────────
juce::Font SynthLookAndFeel::getLabelFont (juce::Label& label)
{
    return label.getFont();
}

} // namespace UI
