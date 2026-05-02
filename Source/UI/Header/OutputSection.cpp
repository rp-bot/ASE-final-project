#include "OutputSection.h"
#include "UI/Common/SynthLookAndFeel.h"
#include "Parameters/ParameterIDs.h"

namespace UI
{

    // Meter constants
    static constexpr float kMeterMinDb = -60.0f;
    static constexpr float kMeterMaxDb = 6.0f;
    static constexpr float kMeterDecayDbPerFrame = 1.5f; // decay speed per timer tick

    static float dbToNorm(float db)
    {
        return juce::jlimit(0.0f, 1.0f, (db - kMeterMinDb) / (kMeterMaxDb - kMeterMinDb));
    }

    //==============================================================================
    OutputSection::OutputSection(juce::AudioProcessorValueTreeState &apvts, std::function<void()> onResetEngineHardOff)
        : apvtsPtr(&apvts),
          onResetEngineHardOff_(std::move(onResetEngineHardOff))
    {
        configureVolumeKnob(volumeKnob);
        configurePanKnob(panKnob);

        addAndMakeVisible(volumeKnob);
        addAndMakeVisible(panKnob);
        addAndMakeVisible(resetEngineButton);
        addAndMakeVisible(dbReadoutLabel);

        dbReadoutLabel.setJustificationType(juce::Justification::centred);
        dbReadoutLabel.setText("-inf dB", juce::dontSendNotification);
        dbReadoutLabel.setFont(juce::Font("Helvetica Neue", 12.0f, juce::Font::plain));

        gainAttachment = std::make_unique<SliderAttachment>(apvts, ParameterIDs::outputGain, volumeKnob.getSlider());
        panAttachment = std::make_unique<SliderAttachment>(apvts, ParameterIDs::outputPan, panKnob.getSlider());

        // Re-apply display format after APVTS attachment (attachment can override text formatting).
        volumeKnob.getSlider().textFromValueFunction = [] (double v)
        {
            return juce::String (v, 1) + " dB";
        };
        volumeKnob.getSlider().valueFromTextFunction = [] (const juce::String& text)
        {
            auto t = text.trim();
            if (t.endsWithIgnoreCase ("db"))
                t = t.dropLastCharacters (2).trimEnd();
            return juce::jlimit (-60.0, 6.0, t.getDoubleValue());
        };

        panKnob.getSlider().textFromValueFunction = [] (double v)
        {
            return juce::String (v, 2);
        };
        panKnob.getSlider().valueFromTextFunction = [] (const juce::String& text)
        {
            return juce::jlimit(-1.0, 1.0, text.getDoubleValue());
        };

        volumeKnob.refreshValueText();
        panKnob.refreshValueText();
        resetEngineButton.onClick = [this]()
        {
            if (onResetEngineHardOff_ != nullptr)
                onResetEngineHardOff_();
        };
        resetEngineButton.setButtonText("Reset Engine");
        resetEngineButton.setColour(juce::TextButton::buttonColourId, SynthLookAndFeel::panelBorder());
        resetEngineButton.setColour(juce::TextButton::textColourOffId, SynthLookAndFeel::textPrimary());
        resetEngineButton.setEnabled(false);
        resetEngineButton.setVisible(false);

        startTimerHz(30); // repaint meter at 30 fps
    }

    OutputSection::~OutputSection()
    {
        stopTimer();
    }

    void OutputSection::setMeterLevels(float leftDb, float rightDb)
    {
        meterLeftDb.store(leftDb, std::memory_order_relaxed);
        meterRightDb.store(rightDb, std::memory_order_relaxed);
    }

    void OutputSection::setEngineHardOffState(bool isHardOff)
    {
        resetEngineButton.setEnabled(isHardOff);
        resetEngineButton.setVisible(isHardOff);
        resetEngineButton.setColour(juce::TextButton::buttonColourId,
                                    isHardOff ? juce::Colours::red : SynthLookAndFeel::panelBorder());
        resetEngineButton.setColour(juce::TextButton::textColourOffId,
                                    isHardOff ? juce::Colours::white : SynthLookAndFeel::textPrimary());
    }

    void OutputSection::timerCallback()
    {
        const float newL = meterLeftDb.load(std::memory_order_relaxed);
        const float newR = meterRightDb.load(std::memory_order_relaxed);

        // Peak hold with decay
        displayLeftDb = std::max(newL, displayLeftDb - kMeterDecayDbPerFrame);
        displayRightDb = std::max(newR, displayRightDb - kMeterDecayDbPerFrame);

        // Update dB readout text (show louder channel)
        const float peakDb = std::max(displayLeftDb, displayRightDb);
        if (peakDb <= kMeterMinDb)
            dbReadoutLabel.setText("-inf dB", juce::dontSendNotification);
        else
            dbReadoutLabel.setText(juce::String(peakDb, 1) + " dB", juce::dontSendNotification);

        repaint();
    }

    void OutputSection::paint(juce::Graphics &g)
    {
        const bool compactLayout = getHeight() < 130;

        // Panel border
        g.setColour(SynthLookAndFeel::panelBorder());
        g.drawRect(getLocalBounds(), 1);

        // calculate meter bar areas
        auto bounds = getLocalBounds().reduced(6);
        const int colW = bounds.getWidth() / 3;

        // volume | pan | meter (left to right)
        bounds.removeFromLeft(colW); // volume
        bounds.removeFromLeft(4);    // gap
        bounds.removeFromLeft(colW); // pan
        bounds.removeFromLeft(4);    // gap

        // meter column area (right-most)
        auto meterCol = bounds;
        meterCol.removeFromTop(compactLayout ? 6 : 18);
        meterCol.removeFromBottom(compactLayout ? 18 : 20);

        const int barGap = 4;
        const int barW = compactLayout ? 4 : 5;
        const int totalBarsW = (barW * 2) + barGap;
        if (meterCol.getWidth() > totalBarsW)
            meterCol = meterCol.withSizeKeepingCentre (totalBarsW, meterCol.getHeight());

        auto drawBar = [&](juce::Rectangle<int> area, float db)
        {
            g.setColour(SynthLookAndFeel::arcTrack());
            g.fillRect(area);

            const float norm = dbToNorm(db);
            const int fillH = juce::roundToInt(area.getHeight() * norm);
            auto filled = area.removeFromBottom(fillH);

            if (db > 0.0f)
                g.setColour(juce::Colours::red);
            else if (db < -12.0f)
                g.setColour(juce::Colours::limegreen);
            else if (db < -3.0f)
                g.setColour(juce::Colours::yellow);
            else
                g.setColour(juce::Colours::orangered);

            g.fillRect(filled);
        };

        auto leftBar = meterCol.removeFromLeft(barW);
        meterCol.removeFromLeft(barGap);
        auto rightBar = meterCol.removeFromLeft(barW);

        drawBar(leftBar, displayLeftDb);
        drawBar(rightBar, displayRightDb);
    }

    void OutputSection::resized()
    {
        const bool compactLayout = getHeight() < 130;
        auto inner = getLocalBounds().reduced(6);
        const int colW = inner.getWidth() / 3;

        // left: volume
        auto faderCol = inner.removeFromLeft(colW);
        inner.removeFromLeft(4);

        // center: pan
        auto panCol = inner.removeFromLeft(colW);
        inner.removeFromLeft(4);

        // right: meter
        auto meterCol = inner;

        // --- fader ---
        volumeKnob.setBounds(faderCol);

        // --- pan ---
        panKnob.setBounds(panCol);

        // --- meter ---
        if (compactLayout)
        {
            dbReadoutLabel.setBounds(meterCol.removeFromBottom(18));
            resetEngineButton.setBounds(juce::Rectangle<int>());
        }
        else
        {
            // label at top, readout and reset at bottom, meter bars in the middle
            meterCol.removeFromTop(18); // label height
            const int readoutHeight = 20;
            const int buttonHeight = 24;
            dbReadoutLabel.setBounds(meterCol.removeFromBottom(readoutHeight));
            meterCol.removeFromBottom(4);
            resetEngineButton.setBounds(meterCol.removeFromBottom(buttonHeight));
        }

    }

    //==============================================================================
    void OutputSection::configureVolumeKnob(LabelledKnob &knob)
    {
        knob.setNameLabelText("Volume");
        knob.setValueFormatter(nullptr);
        auto &slider = knob.getSlider();
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::textBoxOutlineColourId, SynthLookAndFeel::panelBorder());
        slider.setRange(-60.0, 6.0, 0.1);
        slider.setNumDecimalPlacesToDisplay(1);
        slider.setTextValueSuffix(" dB");
        slider.setValue(0.0);
        slider.setDoubleClickReturnValue(true, 0.0);
    }

    void OutputSection::configurePanKnob(LabelledKnob &knob)
    {
        knob.setNameLabelText("Pan");
        knob.setValueFormatter(nullptr);
        auto &slider = knob.getSlider();
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setColour(juce::Slider::textBoxOutlineColourId, SynthLookAndFeel::panelBorder());
        slider.setRange(-1.0, 1.0, 0.01);
        slider.setNumDecimalPlacesToDisplay(2);
        slider.setValue(0.0);
        slider.setDoubleClickReturnValue(true, 0.0);
        slider.textFromValueFunction = [](double v)
        {
            return juce::String(v, 2);
        };
        slider.valueFromTextFunction = [](const juce::String &text)
        {
            return juce::jlimit(-1.0, 1.0, text.getDoubleValue());
        };
    }

} // namespace UI