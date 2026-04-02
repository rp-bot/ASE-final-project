#include "OscillatorModuleComponent.h"
#include "../Parameters/ParameterIDs.h"
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
}

namespace UI
{
OscillatorModuleComponent::OscillatorModuleComponent (juce::AudioProcessorValueTreeState& apvts,
                                                      int cornerIndex,
                                                      const juce::String& titleText,
                                                      juce::Colour accentColour)
    : corner (cornerIndex), accent (accentColour)
{
    titleLabel.setText (titleText, juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setColour (juce::Label::textColourId, accent.brighter (0.3f));
    addAndMakeVisible (titleLabel);

    auto styleTab = [this] (juce::TextButton& b)
    {
        b.setColour (juce::TextButton::buttonColourId, accent.withAlpha (0.12f));
        b.setColour (juce::TextButton::buttonOnColourId, accent.withAlpha (0.4f));
        b.setColour (juce::TextButton::textColourOffId, juce::Colours::lightgrey);
        b.setColour (juce::TextButton::textColourOnId, juce::Colours::white);
    };
    styleTab (tabSynth);
    styleTab (tabFilter);
    styleTab (tabAmp);
    tabSynth.onClick = [this] { setPage (Page::Synth); };
    tabFilter.onClick = [this] { setPage (Page::Filter); };
    tabAmp.onClick = [this] { setPage (Page::Amp); };
    addAndMakeVisible (tabSynth);
    addAndMakeVisible (tabFilter);
    addAndMakeVisible (tabAmp);

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

    for (int paramIndex : kSynthKnobIndices)
    {
        auto& slider = synthSliders[static_cast<size_t> (paramIndex)];
        auto& label = synthLabels[static_cast<size_t> (paramIndex)];
        static constexpr std::array<const char*, synthParams> kSynthNames
        {
            "Level", "Detune", "Wave", "Coarse", "Fine", "Pan"
        };
        configureRotarySlider (slider, label, kSynthNames[static_cast<size_t> (paramIndex)]);
        slider.setColour (juce::Slider::rotarySliderOutlineColourId, accent.withAlpha (0.25f));
        slider.setColour (juce::Slider::rotarySliderFillColourId, accent.withAlpha (0.9f));
        label.setColour (juce::Label::textColourId, juce::Colours::whitesmoke.withAlpha (0.9f));
        addChildComponent (slider);
        addChildComponent (label);

        synthAttachments[static_cast<size_t> (paramIndex)] =
            std::make_unique<SliderAttachment> (apvts, getSynthParameterId (paramIndex), slider);
    }

    for (int i = 0; i < filterParams; ++i)
    {
        configureRotarySlider (filterSliders[static_cast<size_t> (i)], filterLabels[static_cast<size_t> (i)],
                               kFilterNames[static_cast<size_t> (i)]);
        filterSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderOutlineColourId,
                                                          accent.withAlpha (0.25f));
        filterSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId,
                                                          accent.withAlpha (0.9f));
        filterLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId,
                                                         juce::Colours::whitesmoke.withAlpha (0.9f));
        addChildComponent (filterSliders[static_cast<size_t> (i)]);
        addChildComponent (filterLabels[static_cast<size_t> (i)]);
        filterAttachments[static_cast<size_t> (i)] =
            std::make_unique<SliderAttachment> (apvts, getFilterParameterId (i), filterSliders[static_cast<size_t> (i)]);
    }

    for (int i = 0; i < ampParams; ++i)
    {
        configureRotarySlider (ampSliders[static_cast<size_t> (i)], ampLabels[static_cast<size_t> (i)],
                               kAmpNames[static_cast<size_t> (i)]);
        ampSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderOutlineColourId,
                                                      accent.withAlpha (0.25f));
        ampSliders[static_cast<size_t> (i)].setColour (juce::Slider::rotarySliderFillColourId,
                                                       accent.withAlpha (0.9f));
        ampLabels[static_cast<size_t> (i)].setColour (juce::Label::textColourId,
                                                      juce::Colours::whitesmoke.withAlpha (0.9f));
        addChildComponent (ampSliders[static_cast<size_t> (i)]);
        addChildComponent (ampLabels[static_cast<size_t> (i)]);
        ampAttachments[static_cast<size_t> (i)] =
            std::make_unique<SliderAttachment> (apvts, getAmpParameterId (i), ampSliders[static_cast<size_t> (i)]);
    }

    setPage (Page::Synth);
}

OscillatorModuleComponent::~OscillatorModuleComponent()
{
    waveformSelector.setLookAndFeel (nullptr);
}

void OscillatorModuleComponent::paint (juce::Graphics& g)
{
    g.setColour (accent.withAlpha (0.08f));
    g.fillRect (getLocalBounds().reduced (1));
    g.setColour (accent.withAlpha (0.7f));
    g.drawRect (getLocalBounds(), 1);
}

void OscillatorModuleComponent::resized()
{
    auto inner = getLocalBounds().reduced (4);
    auto header = inner.removeFromTop (22);
    titleLabel.setBounds (header.removeFromLeft (juce::jmin (88, header.getWidth() / 2)));
    header.removeFromLeft (6);
    const int tabW = juce::jmin (52, (header.getWidth() - 12) / 3);
    tabSynth.setBounds (header.removeFromLeft (tabW));
    header.removeFromLeft (4);
    tabFilter.setBounds (header.removeFromLeft (tabW));
    header.removeFromLeft (4);
    tabAmp.setBounds (header.removeFromLeft (tabW));

    inner.removeFromTop (4);
    if (activePage == Page::Synth)
        layoutSynthPage (inner);
    else if (activePage == Page::Filter)
        layoutFilterPage (inner);
    else
        layoutAmpPage (inner);
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
    {
        synthSliders[static_cast<size_t> (paramIndex)].setVisible (synth);
        synthLabels[static_cast<size_t> (paramIndex)].setVisible (synth);
    }

    for (int i = 0; i < filterParams; ++i)
    {
        filterSliders[static_cast<size_t> (i)].setVisible (filt);
        filterLabels[static_cast<size_t> (i)].setVisible (filt);
    }

    for (int i = 0; i < ampParams; ++i)
    {
        ampSliders[static_cast<size_t> (i)].setVisible (amp);
        ampLabels[static_cast<size_t> (i)].setVisible (amp);
    }

    refreshTabColours();
    resized();
}

void OscillatorModuleComponent::refreshTabColours()
{
    auto apply = [this] (juce::TextButton& b, bool selected)
    {
        b.setColour (juce::TextButton::buttonColourId,
                     selected ? accent.withAlpha (0.38f) : accent.withAlpha (0.12f));
        b.setColour (juce::TextButton::textColourOffId,
                     selected ? juce::Colours::white : juce::Colours::lightgrey);
    };
    apply (tabSynth, activePage == Page::Synth);
    apply (tabFilter, activePage == Page::Filter);
    apply (tabAmp, activePage == Page::Amp);
}

void OscillatorModuleComponent::layoutSynthPage (juce::Rectangle<int> area)
{
    constexpr int knobGap = 4;
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

        auto sliderArea = knobArea.removeFromTop (juce::jmax (32, knobArea.getHeight() - 14));
        synthSliders[static_cast<size_t> (paramIndex)].setBounds (sliderArea);
        synthLabels[static_cast<size_t> (paramIndex)].setBounds (knobArea);
    }
}

void OscillatorModuleComponent::layoutFilterPage (juce::Rectangle<int> area)
{
    constexpr int gap = 4;
    const int n = filterParams;
    const int cellW = (area.getWidth() - gap * (n - 1)) / n;
    for (int i = 0; i < n; ++i)
    {
        auto cell = area.removeFromLeft (cellW);
        if (i < n - 1)
            area.removeFromLeft (gap);
        auto sliderArea = cell.removeFromTop (juce::jmax (32, cell.getHeight() - 14));
        filterSliders[static_cast<size_t> (i)].setBounds (sliderArea);
        filterLabels[static_cast<size_t> (i)].setBounds (cell);
    }
}

void OscillatorModuleComponent::layoutAmpPage (juce::Rectangle<int> area)
{
    constexpr int gap = 4;
    const int n = ampParams;
    const int cellW = (area.getWidth() - gap * (n - 1)) / n;
    for (int i = 0; i < n; ++i)
    {
        auto cell = area.removeFromLeft (cellW);
        if (i < n - 1)
            area.removeFromLeft (gap);
        auto sliderArea = cell.removeFromTop (juce::jmax (28, cell.getHeight() - 12));
        ampSliders[static_cast<size_t> (i)].setBounds (sliderArea);
        ampLabels[static_cast<size_t> (i)].setBounds (cell);
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
    switch (paramIndex)
    {
        case 0: return ParameterIDs::cornerFilterCutoff (corner);
        case 1: return ParameterIDs::cornerFilterResonance (corner);
        case 2: return ParameterIDs::cornerFilterKeyTrack (corner);
        case 3: return ParameterIDs::cornerFilterDrive (corner);
        default: break;
    }

    jassertfalse;
    return {};
}

juce::String OscillatorModuleComponent::getAmpParameterId (int paramIndex) const
{
    switch (paramIndex)
    {
        case 0: return ParameterIDs::cornerAmpAttack (corner);
        case 1: return ParameterIDs::cornerAmpDecay (corner);
        case 2: return ParameterIDs::cornerAmpSustain (corner);
        case 3: return ParameterIDs::cornerAmpRelease (corner);
        case 4: return ParameterIDs::cornerAmpLevel (corner);
        case 5: return ParameterIDs::cornerAmpVelSens (corner);
        default: break;
    }

    jassertfalse;
    return {};
}

void OscillatorModuleComponent::configureRotarySlider (juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    label.setText (text, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void OscillatorModuleComponent::updateWaveformPreviewFromSelector()
{
    const auto selected = juce::jmax (0, waveformSelector.getSelectedItemIndex());
    waveformPreview.setWaveformIndex (selected);
}

void OscillatorModuleComponent::WaveformPreviewComponent::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    g.setColour (juce::Colours::black.withAlpha (0.2f));
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
            case 1: sample = (2.0f * t) - 1.0f; break;
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
