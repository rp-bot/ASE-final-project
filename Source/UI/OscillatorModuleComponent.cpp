#include "OscillatorModuleComponent.h"
#include "../Parameters/ParameterIDs.h"
#include <cmath>

namespace
{
constexpr std::array<const char*, UI::OscillatorModuleComponent::paramsPerModule> kParamNames
{
    "Level", "Detune", "Wave", "Coarse", "Fine", "Pan"
};

constexpr std::array<int, 5> kKnobParamIndices { 0, 1, 3, 4, 5 };
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
    addAndMakeVisible (waveformSelector);

    waveformPreview.setAccentColour (accent);
    addAndMakeVisible (waveformPreview);

    waveformAttachment = std::make_unique<ComboBoxAttachment> (apvts, getCornerParameterId (2), waveformSelector);
    updateWaveformPreviewFromSelector();

    for (int paramIndex : kKnobParamIndices)
    {
        auto& slider = paramSliders[static_cast<size_t> (paramIndex)];
        auto& label = paramLabels[static_cast<size_t> (paramIndex)];
        configureRotarySlider (slider, label, kParamNames[static_cast<size_t> (paramIndex)]);
        slider.setColour (juce::Slider::rotarySliderOutlineColourId, accent.withAlpha (0.25f));
        slider.setColour (juce::Slider::rotarySliderFillColourId, accent.withAlpha (0.9f));
        label.setColour (juce::Label::textColourId, juce::Colours::whitesmoke.withAlpha (0.9f));
        addAndMakeVisible (slider);
        addAndMakeVisible (label);

        paramAttachments[static_cast<size_t> (paramIndex)] =
            std::make_unique<SliderAttachment> (apvts, getCornerParameterId (paramIndex), slider);
    }
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
    titleLabel.setBounds (inner.removeFromTop (16));
    inner.removeFromTop (2);

    constexpr int knobGap = 4;
    const auto knobCount = static_cast<int> (kKnobParamIndices.size());
    const auto totalCells = knobCount + 1; // one cell reserved for waveform preview
    const auto cellWidth = (inner.getWidth() - (knobGap * (totalCells - 1))) / totalCells;

    auto previewArea = inner.removeFromLeft (cellWidth);
    auto waveUnit = previewArea.reduced (0, 2);
    auto selectorArea = waveUnit.removeFromTop (20);
    waveformSelector.setBounds (selectorArea);
    waveUnit.removeFromTop (2);
    waveformPreview.setBounds (waveUnit);
    inner.removeFromLeft (knobGap);

    for (int index = 0; index < knobCount; ++index)
    {
        const auto paramIndex = kKnobParamIndices[static_cast<size_t> (index)];
        auto knobArea = inner.removeFromLeft (cellWidth);
        if (index < knobCount - 1)
            inner.removeFromLeft (knobGap);

        auto sliderArea = knobArea.removeFromTop (juce::jmax (32, knobArea.getHeight() - 14));
        paramSliders[static_cast<size_t> (paramIndex)].setBounds (sliderArea);
        paramLabels[static_cast<size_t> (paramIndex)].setBounds (knobArea);
    }
}

juce::String OscillatorModuleComponent::getCornerParameterId (int paramIndex) const
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
            case 0: sample = std::sin (phase); break;                                     // sine
            case 1: sample = (2.0f * t) - 1.0f; break;                                    // saw
            case 2: sample = (t < 0.5f) ? 1.0f : -1.0f; break;                            // square
            case 3: sample = (2.0f * std::abs (2.0f * t - 1.0f)) - 1.0f; break;          // triangle
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
    const auto bounds = juce::Rectangle<float> (0.0f, 0.0f, static_cast<float> (width), static_cast<float> (height)).reduced (0.5f);
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
