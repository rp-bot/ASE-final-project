#include "OutputSection.h"
#include "../Parameters/ParameterIDs.h"

namespace UI
{

// Meter constants
static constexpr float kMeterMinDb = -60.0f;
static constexpr float kMeterMaxDb =   6.0f;
static constexpr float kMeterDecayDbPerFrame = 1.5f; // decay speed per timer tick

static float dbToNorm (float db)
{
    return juce::jlimit (0.0f, 1.0f, (db - kMeterMinDb) / (kMeterMaxDb - kMeterMinDb));
}

//==============================================================================
OutputSection::OutputSection (juce::AudioProcessorValueTreeState& apvts)
    : apvtsPtr (&apvts)
{
    configureGainKnob (gainKnob, faderLabel);
    configurePanKnob (panSlider,  panLabel);
    
    addAndMakeVisible (gainKnob);
    addAndMakeVisible (faderLabel);
    addAndMakeVisible (panSlider);
    addAndMakeVisible (panLabel);
    addAndMakeVisible (dbReadoutLabel);

    dbReadoutLabel.setJustificationType (juce::Justification::centred);
    dbReadoutLabel.setText ("-inf dB", juce::dontSendNotification);
    dbReadoutLabel.setFont (juce::Font (12.0f));

    gainAttachment = std::make_unique<SliderAttachment> (apvts, ParameterIDs::outputGain, gainKnob);
    panAttachment  = std::make_unique<SliderAttachment> (apvts, ParameterIDs::outputPan,  panSlider);

    startTimerHz (30); // repaint meter at 30 fps
}

OutputSection::~OutputSection()
{
    stopTimer();
}

void OutputSection::setMeterLevels (float leftDb, float rightDb)
{
    meterLeftDb.store  (leftDb,  std::memory_order_relaxed);
    meterRightDb.store (rightDb, std::memory_order_relaxed);
}

void OutputSection::timerCallback()
{
    const float newL = meterLeftDb.load  (std::memory_order_relaxed);
    const float newR = meterRightDb.load (std::memory_order_relaxed);

    // Peak hold with decay
    displayLeftDb  = std::max (newL, displayLeftDb  - kMeterDecayDbPerFrame);
    displayRightDb = std::max (newR, displayRightDb - kMeterDecayDbPerFrame);

    // Update dB readout text (show louder channel)
    const float peakDb = std::max (displayLeftDb, displayRightDb);
    if (peakDb <= kMeterMinDb)
        dbReadoutLabel.setText ("-inf dB", juce::dontSendNotification);
    else
        dbReadoutLabel.setText (juce::String (peakDb, 1) + " dB", juce::dontSendNotification);

    repaint();
}

void OutputSection::paint (juce::Graphics& g)
{
    // Panel border
    g.setColour (juce::Colours::grey.withAlpha (0.6f));
    g.drawRect (getLocalBounds(), 1);

    // calculate meter bar areas
    auto bounds = getLocalBounds().reduced (6);
    const int colW = bounds.getWidth() / 3;

    bounds.removeFromLeft (colW);
    bounds.removeFromLeft (4);

    // calculate meter column area
    auto meterCol = bounds.removeFromLeft (colW - 4);
    meterCol.removeFromTop (18);       // label height
    meterCol.removeFromBottom (20);    // readout height

    const int barW = (meterCol.getWidth() - 4) / 2;

    auto drawBar = [&] (juce::Rectangle<int> area, float db)
    {
        g.setColour (juce::Colours::darkgrey.withAlpha (0.4f));
        g.fillRect (area);

        const float norm  = dbToNorm (db);
        const int   fillH = juce::roundToInt (area.getHeight() * norm);
        auto        filled = area.removeFromBottom (fillH);

        if (db < -12.0f)
            g.setColour (juce::Colours::limegreen);
        else if (db < -3.0f)
            g.setColour (juce::Colours::yellow);
        else
            g.setColour (juce::Colours::orangered);

        g.fillRect (filled);
    };

    auto leftBar = meterCol.removeFromLeft (barW);
    meterCol.removeFromLeft (4);
    auto rightBar = meterCol.removeFromLeft (barW);

    drawBar (leftBar,  displayLeftDb);
    drawBar (rightBar, displayRightDb);
}

void OutputSection::resized()
{
    auto inner = getLocalBounds().reduced (6);
    const int colW = inner.getWidth() / 3;

    // 左列：fader
    auto faderCol = inner.removeFromLeft (colW);
    inner.removeFromLeft (4);

    // 中列：meter
    auto meterCol = inner.removeFromLeft (colW - 4);
    inner.removeFromLeft (4);

    // 右列：pan
    auto panCol = inner;

    // --- fader ---
    faderLabel.setBounds (faderCol.removeFromTop (18));
    const int knobSize = juce::jmin (faderCol.getWidth(), faderCol.getHeight());
    gainKnob.setBounds (faderCol.withSizeKeepingCentre (knobSize, knobSize));

    // --- meter ---
    // label at top, readout at bottom, meter bars in the middle
    meterCol.removeFromTop (18);  // label height
    dbReadoutLabel.setBounds (meterCol.removeFromBottom (20));

    // --- pan ---
    panLabel.setBounds (panCol.removeFromTop (18));
    panSlider.setBounds (panCol.withSizeKeepingCentre (knobSize, knobSize));
}

//==============================================================================
void OutputSection::configureGainKnob (juce::Slider& slider, juce::Label& label)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 18);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::grey.withAlpha (0.6f));
    slider.setRange (-60.0, 6.0, 0.1);
    slider.setValue (0.0);
    slider.setDoubleClickReturnValue (true, 0.0);
    label.setText ("Volume", juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

void OutputSection::configurePanKnob (juce::Slider& slider, juce::Label& label)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 56, 18);
    slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::grey.withAlpha (0.6f));
    slider.setRange (-1.0, 1.0, 0.01);
    slider.setValue (0.0);
    slider.setDoubleClickReturnValue (true, 0.0);
    label.setText ("Pan", juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
}

} // namespace UI