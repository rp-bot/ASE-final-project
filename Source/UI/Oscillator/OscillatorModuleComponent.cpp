#include "OscillatorModuleComponent.h"
#include "UI/Common/SynthLookAndFeel.h"
#include "Parameters/ParameterIDs.h"
#include <cmath>

namespace
{
constexpr std::array<int, 5> kSynthKnobIndices { 0, 1, 3, 4, 5 };

constexpr std::array<const char*, UI::OscillatorModuleComponent::filterParams> kFilterNames
{
    "Cutoff", "Res", "Key", "Drive"
};

constexpr std::array<const char*, UI::OscillatorModuleComponent::ampParams> kAmpNames
{
    "Attack", "Decay", "Sustain", "Release", "Level", "Vel"
};

/** Slider text decimals after APVTS attachment (matches sensible edit precision; avoids long float tails). */
constexpr int synthKnobDecimalPlaces (int synthParamIndex) noexcept
{
    switch (synthParamIndex)
    {
        case 3: return 0; // coarse: integer semitones
        default: return 2; // level, detune, fine, pan (0.01 steps)
    }
}

constexpr int filterKnobDecimalPlaces (int filterParamIndex) noexcept
{
    return filterParamIndex == 0 ? 0 : 2; // cutoff Hz vs 0–1 style params
}

constexpr int ampKnobDecimalPlaces (int ampParamIndex) noexcept
{
    if (ampParamIndex == 0 || ampParamIndex == 1 || ampParamIndex == 3)
        return 3; // attack / decay / release: 0.001 s steps

    return 2;
}

juce::String formatNumericValue (double value, int decimals)
{
    if (std::abs (value) < 0.0000001)
        value = 0.0;

    return juce::String (value, decimals);
}

void applyNumericTextFormatting (juce::Slider& slider, int decimals)
{
    slider.textFromValueFunction = [decimals] (double value)
    {
        return formatNumericValue (value, decimals);
    };
}

double parseNumericWithOptionalUnits (juce::String text)
{
    text = text.trim();

    auto dropSuffix = [&text] (const juce::String& suffix)
    {
        if (text.endsWithIgnoreCase (suffix))
            text = text.dropLastCharacters (suffix.length()).trimEnd();
    };

    if (text.endsWithIgnoreCase ("khz"))
    {
        dropSuffix ("khz");
        return text.getDoubleValue() * 1000.0;
    }

    if (text.endsWithIgnoreCase ("hz"))
        dropSuffix ("hz");
    else if (text.endsWithIgnoreCase ("ms"))
    {
        dropSuffix ("ms");
        return text.getDoubleValue() / 1000.0;
    }
    else if (text.endsWithIgnoreCase ("s"))
        dropSuffix ("s");
    else if (text.endsWithIgnoreCase ("st"))
        dropSuffix ("st");
    else if (text.endsWithIgnoreCase ("ct"))
        dropSuffix ("ct");
    else if (text.endsWithChar ('%'))
    {
        text = text.dropLastCharacters (1).trimEnd();
        return text.getDoubleValue() / 100.0;
    }

    return text.getDoubleValue();
}

void applyFilterTextFormatting (juce::Slider& slider, int filterParamIndex)
{
    slider.textFromValueFunction = [filterParamIndex] (double value)
    {
        if (std::abs (value) < 0.0000001)
            value = 0.0;

        if (filterParamIndex == 0)
            return value >= 1000.0 ? juce::String (value / 1000.0, 2) + " kHz"
                                   : juce::String (value, 0) + " Hz";

        return juce::String (juce::roundToInt (value * 100.0)) + " %";
    };

    slider.valueFromTextFunction = [filterParamIndex] (const juce::String& text)
    {
        const double parsed = parseNumericWithOptionalUnits (text);
        if (filterParamIndex == 0)
            return parsed;

        // Res/Key/Drive display as percent but underlying params are 0..1.
        return parsed;
    };
}

void applyAmpTextFormatting (juce::Slider& slider, int ampParamIndex)
{
    slider.textFromValueFunction = [ampParamIndex] (double value)
    {
        if (std::abs (value) < 0.0000001)
            value = 0.0;

        if (ampParamIndex == 0 || ampParamIndex == 1 || ampParamIndex == 3)
        {
            if (value < 1.0)
                return juce::String (juce::roundToInt (value * 1000.0)) + " ms";
            return juce::String (value, 2) + " s";
        }

        return juce::String (juce::roundToInt (value * 100.0)) + " %";
    };

    slider.valueFromTextFunction = [] (const juce::String& text)
    {
        return parseNumericWithOptionalUnits (text);
    };
}
}

namespace UI
{
void OscillatorModuleComponent::TabLookAndFeel::drawButtonBackground (juce::Graphics& g,
                                                                      juce::Button& button,
                                                                      const juce::Colour& /*backgroundColour*/,
                                                                      bool shouldDrawButtonAsHighlighted,
                                                                      bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat();
    const bool selected = isTabSelected != nullptr && isTabSelected (button);

    if (selected)
    {
        g.setColour (accent);
        g.drawLine (bounds.getX() + 1.0f,
                    bounds.getBottom() - 1.5f,
                    bounds.getRight() - 1.0f,
                    bounds.getBottom() - 1.5f,
                    2.0f);
    }

    if (shouldDrawButtonAsHighlighted || shouldDrawButtonAsDown)
    {
        g.setColour (accent.withAlpha (0.1f));
        g.fillRoundedRectangle (bounds.reduced (1.0f, 0.0f), 2.0f);
    }
}

void OscillatorModuleComponent::TabLookAndFeel::drawButtonText (juce::Graphics& g,
                                                                juce::TextButton& textButton,
                                                                bool shouldDrawButtonAsHighlighted,
                                                                bool shouldDrawButtonAsDown)
{
    const bool selected = isTabSelected != nullptr && isTabSelected (textButton);
    auto col = selected ? accent : SynthLookAndFeel::textDim();

    if (shouldDrawButtonAsHighlighted)
        col = col.brighter (0.12f);

    if (shouldDrawButtonAsDown)
        col = col.brighter (0.08f);

    g.setFont (getTextButtonFont (textButton, textButton.getHeight()));
    g.setColour (col);
    g.drawFittedText (textButton.getButtonText(),
                      textButton.getLocalBounds().reduced (2, 0),
                      juce::Justification::centred,
                      1);
}

juce::Font OscillatorModuleComponent::TabLookAndFeel::getTextButtonFont (juce::TextButton&, int buttonHeight)
{
    juce::ignoreUnused (buttonHeight);
    return juce::Font (10.0f);
}

OscillatorModuleComponent::OscillatorModuleComponent (juce::AudioProcessorValueTreeState& apvts,
                                                      int cornerIndex,
                                                      const juce::String& titleText,
                                                      juce::Colour accentColour)
    : corner (cornerIndex), accent (accentColour)
{
    moduleTitle_ = titleText;

    tabLookAndFeel.setAccent (accent);
    tabLookAndFeel.setTabSelectedPredicate ([this] (const juce::Button& b)
    {
        if (&b == &tabSynth)
            return activePage == Page::Synth;
        if (&b == &tabFilter)
            return activePage == Page::Filter;
        if (&b == &tabAmp)
            return activePage == Page::Amp;

        return false;
    });

    tabSynth.onClick = [this] { setPage (Page::Synth); };
    tabFilter.onClick = [this] { setPage (Page::Filter); };
    tabAmp.onClick = [this] { setPage (Page::Amp); };

    for (auto* tab : { &tabSynth, &tabFilter, &tabAmp })
    {
        tab->setLookAndFeel (&tabLookAndFeel);
        addAndMakeVisible (*tab);
    }

    waveformSelector.addItem ("Sine", 1);
    waveformSelector.addItem ("Saw", 2);
    waveformSelector.addItem ("Square", 3);
    waveformSelector.addItem ("Triangle", 4);
    comboLookAndFeel.setAccent (accent);
    comboLookAndFeel.setColour (juce::PopupMenu::backgroundColourId, accent.darker (0.85f).withAlpha (0.95f));
    comboLookAndFeel.setColour (juce::PopupMenu::textColourId, accent.brighter (0.25f));
    comboLookAndFeel.setColour (juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha (0.45f));
    comboLookAndFeel.setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    waveformSelector.setLookAndFeel (&comboLookAndFeel);
    waveformSelector.setColour (juce::ComboBox::backgroundColourId, accent.withAlpha (0.15f));
    waveformSelector.setColour (juce::ComboBox::outlineColourId, accent.withAlpha (0.75f));
    waveformSelector.setColour (juce::ComboBox::textColourId, accent.brighter (0.25f));
    waveformSelector.setColour (juce::ComboBox::arrowColourId, accent.brighter (0.35f));
    waveformSelector.setColour (juce::PopupMenu::backgroundColourId, accent.darker (0.85f).withAlpha (0.95f));
    waveformSelector.setColour (juce::PopupMenu::textColourId, accent.brighter (0.25f));
    waveformSelector.setColour (juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha (0.45f));
    waveformSelector.setColour (juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
    waveformSelector.onChange = [this] { updateWaveformPreviewFromSelector(); };
    addChildComponent (waveformSelector);

    waveformPreview.setAccentColour (accent);
    addChildComponent (waveformPreview);

    waveformAttachment =
        std::make_unique<ComboBoxAttachment> (apvts, getSynthParameterId (2), waveformSelector);
    updateWaveformPreviewFromSelector();

    static constexpr std::array<const char*, synthParams> kSynthNames
    {
        "Level", "Detune", "Wave", "Coarse", "Fine", "Pan"
    };

    for (int paramIndex : kSynthKnobIndices)
    {
        auto& knob = synthKnobs[static_cast<size_t> (paramIndex)];
        configureRotaryKnob (knob, kSynthNames[static_cast<size_t> (paramIndex)]);
        knob.getSlider().setColour (juce::Slider::rotarySliderOutlineColourId, accent.withAlpha (0.25f));
        knob.getSlider().setColour (juce::Slider::rotarySliderFillColourId, accent);
        knob.getNameLabel().setColour (juce::Label::textColourId, SynthLookAndFeel::textDim());
        knob.getValueLabel().setColour (juce::Label::textColourId, SynthLookAndFeel::textPrimary());
        addChildComponent (knob);

        synthAttachments[static_cast<size_t> (paramIndex)] =
            std::make_unique<SliderAttachment> (apvts, getSynthParameterId (paramIndex), knob.getSlider());
        const int decimals = synthKnobDecimalPlaces (paramIndex);
        knob.getSlider().setNumDecimalPlacesToDisplay (decimals);
        applyNumericTextFormatting (knob.getSlider(), decimals);
        knob.refreshValueText();
    }

    for (int i = 0; i < filterParams; ++i)
    {
        auto& knob = filterKnobs[static_cast<size_t> (i)];
        configureRotaryKnob (knob, kFilterNames[static_cast<size_t> (i)]);
        knob.getSlider().setColour (juce::Slider::rotarySliderOutlineColourId,
                                   accent.withAlpha (0.25f));
        knob.getSlider().setColour (juce::Slider::rotarySliderFillColourId, accent);
        knob.getNameLabel().setColour (juce::Label::textColourId, SynthLookAndFeel::textDim());
        knob.getValueLabel().setColour (juce::Label::textColourId, SynthLookAndFeel::textPrimary());
        addChildComponent (knob);
        filterAttachments[static_cast<size_t> (i)] =
            std::make_unique<SliderAttachment> (apvts, getFilterParameterId (i), knob.getSlider());
        const int decimals = filterKnobDecimalPlaces (i);
        knob.getSlider().setNumDecimalPlacesToDisplay (decimals);
        applyFilterTextFormatting (knob.getSlider(), i);
        knob.refreshValueText();
    }

    for (int i = 0; i < ampParams; ++i)
    {
        auto& knob = ampKnobs[static_cast<size_t> (i)];
        configureRotaryKnob (knob, kAmpNames[static_cast<size_t> (i)]);
        knob.getSlider().setColour (juce::Slider::rotarySliderOutlineColourId,
                                   accent.withAlpha (0.25f));
        knob.getSlider().setColour (juce::Slider::rotarySliderFillColourId, accent);
        knob.getNameLabel().setColour (juce::Label::textColourId, SynthLookAndFeel::textDim());
        knob.getValueLabel().setColour (juce::Label::textColourId, SynthLookAndFeel::textPrimary());
        addChildComponent (knob);
        ampAttachments[static_cast<size_t> (i)] =
            std::make_unique<SliderAttachment> (apvts, getAmpParameterId (i), knob.getSlider());
        const int decimals = ampKnobDecimalPlaces (i);
        knob.getSlider().setNumDecimalPlacesToDisplay (decimals);
        applyAmpTextFormatting (knob.getSlider(), i);
        knob.refreshValueText();
    }

    setPage (Page::Synth);
}

OscillatorModuleComponent::~OscillatorModuleComponent()
{
    tabSynth.setLookAndFeel (nullptr);
    tabFilter.setLookAndFeel (nullptr);
    tabAmp.setLookAndFeel (nullptr);
    waveformSelector.setLookAndFeel (nullptr);
}

void OscillatorModuleComponent::paint (juce::Graphics& g)
{
    g.setColour (accent.withAlpha (0.08f));
    g.fillRect (getLocalBounds().reduced (1));
    g.setColour (accent.withAlpha (0.7f));
    g.drawRect (getLocalBounds(), 1);

    const float ms = juce::jlimit (0.0f, 1.0f,
                                   (static_cast<float> (getWidth()) - 280.0f) / (384.0f - 280.0f));
    const int innerPad = 2 + juce::roundToInt (2.0f * ms);
    constexpr int tabStripH = 14;
    const auto tabArea = getLocalBounds().reduced (innerPad, 0).withHeight (tabStripH).translated (0, 1);
    const int tabLaneW = juce::jlimit (150, 210, juce::roundToInt (static_cast<float> (tabArea.getWidth()) * 0.52f));
    const int tabLaneX = tabArea.getX() + (tabArea.getWidth() - tabLaneW) / 2;
    const auto tabRailArea = tabArea;
    const auto titleArea = tabArea.withTrimmedRight (juce::jmin (tabLaneW, tabArea.getWidth() - 72));

    juce::ignoreUnused (tabRailArea);

    g.setColour (accent.withAlpha (0.72f));
    g.setFont (juce::Font (9.5f));
    g.drawText (moduleTitle_, titleArea.reduced (4, 0), juce::Justification::centredLeft, true);
}

void OscillatorModuleComponent::resized()
{
    // Reduce inner padding as the module shrinks so knobs keep more of their size.
    // Natural module width at default window size is ~384 px; minimum is ~280 px.
    const float ms = juce::jlimit (0.0f, 1.0f,
        (static_cast<float> (getWidth()) - 280.0f) / (384.0f - 280.0f));
    const int innerPad = 2 + juce::roundToInt (2.0f * ms); // 2..4, was fixed 4
    constexpr int tabStripH = 14;
    constexpr int contentTopInset = 16;
    const int tabLaneW = juce::jlimit (150, 210, juce::roundToInt (static_cast<float> (getWidth()) * 0.52f));

    // Slim tab strip overlaps the top border; knobs use full inner height.
    auto tabArea = getLocalBounds().reduced (innerPad, 0).withHeight (tabStripH).translated (0, 1);
    auto tabRight = juce::Rectangle<int> (tabArea.getX() + (tabArea.getWidth() - tabLaneW) / 2,
                                          tabArea.getY(),
                                          tabLaneW,
                                          tabArea.getHeight());

    const int interTab = 8;
    const int tabW = juce::jmax (38, (tabRight.getWidth() - (interTab * 2)) / 3);
    tabSynth.setBounds (tabRight.removeFromLeft (tabW));
    tabRight.removeFromLeft (interTab);
    tabFilter.setBounds (tabRight.removeFromLeft (tabW));
    tabRight.removeFromLeft (interTab);
    tabAmp.setBounds (tabRight.removeFromLeft (tabW));

    auto inner = getLocalBounds().reduced (innerPad);
    inner.removeFromTop (contentTopInset);
    if (activePage == Page::Synth)
        layoutSynthPage (inner);
    else if (activePage == Page::Filter)
        layoutFilterPage (inner);
    else
        layoutAmpPage (inner);

    // Tabs overlap the top of the knob area; keep them hit-testable and visible on top.
    tabSynth.toFront (false);
    tabFilter.toFront (false);
    tabAmp.toFront (false);
}

void OscillatorModuleComponent::setPage (Page page)
{
    activePage = page;
    const bool synth = (page == Page::Synth);
    const bool filt = (page == Page::Filter);
    const bool amp = (page == Page::Amp);

    waveformSelector.setVisible (synth);
    waveformPreview.setVisible (synth);
    for (int paramIndex : kSynthKnobIndices)
        synthKnobs[static_cast<size_t> (paramIndex)].setVisible (synth);

    for (int i = 0; i < filterParams; ++i)
        filterKnobs[static_cast<size_t> (i)].setVisible (filt);

    for (int i = 0; i < ampParams; ++i)
        ampKnobs[static_cast<size_t> (i)].setVisible (amp);

    refreshTabColours();
    resized();
}

void OscillatorModuleComponent::refreshTabColours()
{
    tabSynth.repaint();
    tabFilter.repaint();
    tabAmp.repaint();
}

void OscillatorModuleComponent::layoutSynthPage (juce::Rectangle<int> area)
{
    // Reduce gap between cells first so knobs shrink less when module width is small.
    // Natural content width ~364 px (6 cells * ~60 px + 5 gaps * 4 px).
    const float gs = juce::jlimit (0.0f, 1.0f,
        (static_cast<float> (area.getWidth()) - 240.0f) / (364.0f - 240.0f));
    const int knobGap  = 1 + juce::roundToInt (3.0f * gs); // 1..4, was fixed 4
    const auto knobCount = static_cast<int> (kSynthKnobIndices.size());
    const auto totalCells = knobCount + 1;
    const auto cellWidth = (area.getWidth() - (knobGap * (totalCells - 1))) / totalCells;

    auto previewArea = area.removeFromLeft (cellWidth);
    auto waveUnit = previewArea.reduced (0, 2);
    auto selectorArea = waveUnit.removeFromTop (20);
    waveformSelector.setBounds (selectorArea);
    waveUnit.removeFromTop (2);
    waveformPreview.setBounds (waveUnit);
    area.removeFromLeft (knobGap);

    for (int index = 0; index < knobCount; ++index)
    {
        const auto paramIndex = kSynthKnobIndices[static_cast<size_t> (index)];
        auto knobArea = area.removeFromLeft (cellWidth);
        if (index < knobCount - 1)
            area.removeFromLeft (knobGap);

        synthKnobs[static_cast<size_t> (paramIndex)].setBounds (knobArea);
    }
}

void OscillatorModuleComponent::layoutFilterPage (juce::Rectangle<int> area)
{
    const float gs = juce::jlimit (0.0f, 1.0f,
        (static_cast<float> (area.getWidth()) - 180.0f) / (300.0f - 180.0f));
    const int gap = 1 + juce::roundToInt (3.0f * gs); // 1..4, was fixed 4
    const int n = filterParams;
    const int cellW = (area.getWidth() - gap * (n - 1)) / n;
    for (int i = 0; i < n; ++i)
    {
        auto cell = area.removeFromLeft (cellW);
        if (i < n - 1)
            area.removeFromLeft (gap);
        filterKnobs[static_cast<size_t> (i)].setBounds (cell);
    }
}

void OscillatorModuleComponent::layoutAmpPage (juce::Rectangle<int> area)
{
    const float gs = juce::jlimit (0.0f, 1.0f,
        (static_cast<float> (area.getWidth()) - 200.0f) / (320.0f - 200.0f));
    const int gap = 1 + juce::roundToInt (3.0f * gs); // 1..4, was fixed 4
    const int n = ampParams;
    const int cellW = (area.getWidth() - gap * (n - 1)) / n;
    for (int i = 0; i < n; ++i)
    {
        auto cell = area.removeFromLeft (cellW);
        if (i < n - 1)
            area.removeFromLeft (gap);
        ampKnobs[static_cast<size_t> (i)].setBounds (cell);
    }
}

juce::String OscillatorModuleComponent::getSynthParameterId (int paramIndex) const
{
    switch (paramIndex)
    {
        case 0: return ParameterIDs::cornerLevel (corner);
        case 1: return ParameterIDs::cornerDetune (corner);
        case 2: return ParameterIDs::cornerWaveform (corner);
        case 3: return ParameterIDs::cornerCoarse (corner);
        case 4: return ParameterIDs::cornerFine (corner);
        case 5: return ParameterIDs::cornerPan (corner);
        default: break;
    }

    jassertfalse;
    return {};
}

juce::String OscillatorModuleComponent::getFilterParameterId (int paramIndex) const
{
    return ParameterIDs::cornerFilterById (corner, paramIndex);
}

juce::String OscillatorModuleComponent::getAmpParameterId (int paramIndex) const
{
    return ParameterIDs::cornerAmpById (corner, paramIndex);
}

void OscillatorModuleComponent::configureRotaryKnob (LabelledKnob& knob, const juce::String& text)
{
    knob.setNameLabelText (text);
    knob.setValueFormatter (nullptr);
    knob.setValueEditExtraChars ("kKhHzZmsMSstct% ");
    auto& slider = knob.getSlider();
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
}

void OscillatorModuleComponent::updateWaveformPreviewFromSelector()
{
    const auto selected = juce::jmax (0, waveformSelector.getSelectedItemIndex());
    waveformPreview.setWaveformIndex (selected);
}

void OscillatorModuleComponent::WaveformPreviewComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    g.setColour (SynthLookAndFeel::panelSurface());
    g.fillRoundedRectangle (bounds, 3.0f);
    g.setColour (accent.withAlpha (0.5f));
    g.drawRoundedRectangle (bounds, 3.0f, 1.0f);

    const auto waveArea = bounds.reduced (3.0f);
    juce::Path waveformPath;
    const auto centerY = waveArea.getCentreY();
    const auto leftX = waveArea.getX();
    const auto width = waveArea.getWidth();
    const auto amplitude = waveArea.getHeight() * 0.42f;
    constexpr int numSamples = 96;

    for (int i = 0; i < numSamples; ++i)
    {
        const auto t = static_cast<float> (i) / static_cast<float> (numSamples - 1);
        const auto phase = t * juce::MathConstants<float>::twoPi;
        float sample = 0.0f;

        switch (waveformIndex)
        {
            case 0: sample = std::sin (phase); break;
            case 1:
            {
                // Render two teeth so the vertical reset between cycles is visible.
                // A single saw cycle from -1 to +1 would otherwise look identical
                // to one slope of the triangle wave (a plain diagonal line).
                const auto p = (t * 2.0f) - std::floor (t * 2.0f);
                sample = (2.0f * p) - 1.0f;
                break;
            }
            case 2: sample = (t < 0.5f) ? 1.0f : -1.0f; break;
            case 3: sample = (2.0f * std::abs (2.0f * t - 1.0f)) - 1.0f; break;
            default: sample = std::sin (phase); break;
        }

        const auto x = leftX + (t * width);
        const auto y = centerY - (sample * amplitude);
        if (i == 0)
            waveformPath.startNewSubPath (x, y);
        else
            waveformPath.lineTo (x, y);
    }

    g.setColour (accent.brighter (0.2f));
    g.strokePath (waveformPath, juce::PathStrokeType (1.4f));
}

void OscillatorModuleComponent::AccentComboLookAndFeel::drawComboBox (juce::Graphics& g,
                                                                      int width,
                                                                      int height,
                                                                      bool,
                                                                      int,
                                                                      int,
                                                                      int,
                                                                      int,
                                                                      juce::ComboBox&)
{
    const auto bounds =
        juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height)).reduced (0.5f);
    g.setColour (accent.withAlpha (0.15f));
    g.fillRoundedRectangle (bounds, 3.0f);
    g.setColour (accent.withAlpha (0.7f));
    g.drawRoundedRectangle (bounds, 3.0f, 1.0f);

    juce::Path arrow;
    const auto cx = bounds.getRight() - 10.0f;
    const auto cy = bounds.getCentreY();
    arrow.startNewSubPath (cx - 4.0f, cy - 2.0f);
    arrow.lineTo (cx + 4.0f, cy - 2.0f);
    arrow.lineTo (cx, cy + 3.0f);
    arrow.closeSubPath();
    g.setColour (accent.brighter (0.35f));
    g.fillPath (arrow);
}

juce::Font OscillatorModuleComponent::AccentComboLookAndFeel::getComboBoxFont (juce::ComboBox& box)
{
    juce::ignoreUnused (box);
    return juce::Font (12.0f);
}

void OscillatorModuleComponent::AccentComboLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (1, 1, box.getWidth() - 18, box.getHeight() - 2);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (juce::Justification::centredLeft);
}
} // namespace UI
