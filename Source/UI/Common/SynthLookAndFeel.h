#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{

/**
 * Global LookAndFeel for the Volumetric Synth plugin.
 *
 * Palette (Soothe-style, light/airy):
 *   Background   0xfff0ede8  warm cream
 *   Panel        0xfffaf8f5  raised surface
 *   Border       0xffddd8d2  soft warm grey
 *   Text primary 0xff1c1a20  near-black
 *   Text dim     0xff7a7880  medium warm grey
 *   Knob body    0xffe8e4de  flat, near-BG
 *   Arc track    0xffcdc7c0  quiet track
 *   Default acc  0xff00a896  teal (non-oscillator controls)
 *
 * Per-oscillator accent colours are injected by each module via
 * slider.setColour(Slider::rotarySliderFillColourId, accent).
 * This LAF reads that colour for the active arc; everything else
 * falls back to the palette above.
 */
class SynthLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SynthLookAndFeel();
    ~SynthLookAndFeel() override = default;

    // Rotary knobs — flat body, coloured arc
    void drawRotarySlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPosProportional,
                           float rotaryStartAngle,
                           float rotaryEndAngle,
                           juce::Slider&) override;

    // Linear sliders — thin rounded track + flat thumb
    void drawLinearSlider (juce::Graphics&,
                           int x, int y, int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           juce::Slider::SliderStyle,
                           juce::Slider&) override;

    // Buttons — rounded rect with border; teal fill when active
    void drawButtonBackground (juce::Graphics&,
                               juce::Button&,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    // Labels — Helvetica Neue at the component's current font size
    juce::Font getLabelFont (juce::Label&) override;

    // Expose palette colours for use by components that draw themselves
    static juce::Colour background()   { return juce::Colour (0xfff0ede8); }
    static juce::Colour panelSurface() { return juce::Colour (0xfffaf8f5); }
    static juce::Colour panelBorder()  { return juce::Colour (0xffddd8d2); }
    static juce::Colour textPrimary()  { return juce::Colour (0xff1c1a20); }
    static juce::Colour textDim()      { return juce::Colour (0xff7a7880); }
    static juce::Colour arcTrack()     { return juce::Colour (0xffcdc7c0); }
    static juce::Colour knobBody()     { return juce::Colour (0xffe8e4de); }
    static juce::Colour teal()         { return juce::Colour (0xff00a896); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthLookAndFeel)
};

} // namespace UI
