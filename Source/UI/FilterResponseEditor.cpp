#include "FilterResponseEditor.h"

#include <cmath>

namespace UI
{
namespace
{
constexpr float kCutoffMin = 20.0f;
constexpr float kCutoffMax = 20000.0f;
constexpr float kResMin = 0.0f;
constexpr float kResMax = 1.0f;
constexpr float kKeyMin = 0.0f;
constexpr float kKeyMax = 1.0f;
constexpr float kDriveMin = 0.0f;
constexpr float kDriveMax = 1.0f;
} // namespace

FilterResponseEditor::FilterResponseEditor (juce::AudioProcessorValueTreeState& state,
                                            std::array<juce::String, 4> parameterIds,
                                            juce::Colour accentColour)
    : apvts (state), ids (std::move (parameterIds)), accent (accentColour)
{
    refreshFromParameters();
    startTimerHz (30);
}

void FilterResponseEditor::paint (juce::Graphics& g)
{
    const auto plot = getPlotBounds();
    g.setColour (juce::Colours::black.withAlpha (0.25f));
    g.fillRoundedRectangle (plot, 4.0f);
    g.setColour (accent.withAlpha (0.55f));
    g.drawRoundedRectangle (plot, 4.0f, 1.0f);

    // Frequency decade guides.
    constexpr std::array<float, 4> guides { 100.0f, 1000.0f, 10000.0f, 20000.0f };
    g.setColour (accent.withAlpha (0.2f));
    for (const auto hz : guides)
    {
        const auto nx = std::log10 (hz / kCutoffMin) / std::log10 (kCutoffMax / kCutoffMin);
        const auto x = plot.getX() + (nx * plot.getWidth());
        g.drawVerticalLine (juce::roundToInt (x), plot.getY(), plot.getBottom());
    }
    g.setFont (10.0f);
    g.setColour (juce::Colours::whitesmoke.withAlpha (0.75f));
    for (const auto hz : guides)
    {
        const auto nx = std::log10 (hz / kCutoffMin) / std::log10 (kCutoffMax / kCutoffMin);
        const auto x = plot.getX() + (nx * plot.getWidth());
        juce::Rectangle<int> labelArea (juce::roundToInt (x - 22.0f),
                                        juce::roundToInt (plot.getBottom() - 14.0f),
                                        44, 12);
        const auto label = hz >= 1000.0f ? juce::String (hz / 1000.0f, 0) + "k" : juce::String (hz, 0);
        g.drawFittedText (label, labelArea, juce::Justification::centred, 1);
    }

    const auto cutoffNorm = std::log10 (values[0] / kCutoffMin) / std::log10 (kCutoffMax / kCutoffMin);
    const auto qBoost = values[1];
    const auto driveBoost = values[3] * 8.0f;
    const auto slopeDepth = 52.0f + values[2] * 28.0f;

    juce::Path response;
    constexpr int samples = 128;
    for (int i = 0; i < samples; ++i)
    {
        const auto nx = static_cast<float> (i) / static_cast<float> (samples - 1);
        const auto x = plot.getX() + nx * plot.getWidth();

        const auto dx = nx - cutoffNorm;
        const auto postCut = juce::jmax (0.0f, dx);
        const auto lowPassShape = -(std::pow (postCut * 1.85f, 1.25f)) * slopeDepth;
        const auto resonancePeak = std::exp (-120.0f * dx * dx) * (qBoost * 12.0f);
        const auto db = juce::jlimit (-24.0f, 12.0f, lowPassShape + resonancePeak + driveBoost);
        const auto y = juce::jmap (db, 12.0f, -24.0f, plot.getY(), plot.getBottom());

        if (i == 0)
            response.startNewSubPath (x, y);
        else
            response.lineTo (x, y);
    }

    g.setColour (accent.brighter (0.2f));
    g.strokePath (response, juce::PathStrokeType (1.5f));

    if (spectrumBins != nullptr)
    {
        juce::Path spectrumPath;
        for (size_t i = 0; i < spectrumBins->size(); ++i)
        {
            const auto t = static_cast<float> (i) / static_cast<float> (spectrumBins->size() - 1);
            const auto x = plot.getX() + t * plot.getWidth();
            const auto mag = (*spectrumBins)[i].load (std::memory_order_relaxed);
            const auto y = juce::jmap (mag, 0.0f, 1.0f, plot.getBottom(), plot.getY());
            if (i == 0)
                spectrumPath.startNewSubPath (x, y);
            else
                spectrumPath.lineTo (x, y);
        }

        g.setColour (juce::Colours::cyan.withAlpha (0.8f));
        g.strokePath (spectrumPath, juce::PathStrokeType (1.1f));
    }

    const auto main = getMainHandlePosition();
    g.setColour (accent.withAlpha (0.85f));
    g.fillEllipse (juce::Rectangle<float> (10.0f, 10.0f).withCentre (main));
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.drawEllipse (juce::Rectangle<float> (10.0f, 10.0f).withCentre (main), 1.0f);
    g.setColour (juce::Colours::whitesmoke.withAlpha (0.95f));
    g.setFont (10.5f);
    const auto cutoffLabel = values[0] >= 1000.0f
        ? juce::String (values[0] / 1000.0f, 2) + " kHz"
        : juce::String (values[0], 0) + " Hz";
    juce::Rectangle<int> cutoffArea (juce::roundToInt (main.x + 8.0f),
                                     juce::roundToInt (main.y - 16.0f),
                                     64, 14);
    g.drawFittedText (cutoffLabel, cutoffArea, juce::Justification::centredLeft, 1);

    const auto keyBounds = getMiniControlBounds (true);
    const auto driveBounds = getMiniControlBounds (false);
    g.setColour (accent.withAlpha (0.14f));
    g.fillRoundedRectangle (keyBounds, 3.0f);
    g.fillRoundedRectangle (driveBounds, 3.0f);
    g.setColour (accent.withAlpha (0.6f));
    g.drawRoundedRectangle (keyBounds, 3.0f, 1.0f);
    g.drawRoundedRectangle (driveBounds, 3.0f, 1.0f);

    const auto keyX = juce::jmap (values[2], 0.0f, 1.0f, keyBounds.getX(), keyBounds.getRight());
    const auto driveX = juce::jmap (values[3], 0.0f, 1.0f, driveBounds.getX(), driveBounds.getRight());
    g.setColour (accent.brighter (0.4f));
    g.drawLine (keyX, keyBounds.getY(), keyX, keyBounds.getBottom(), 2.0f);
    g.drawLine (driveX, driveBounds.getY(), driveX, driveBounds.getBottom(), 2.0f);

    g.setColour (juce::Colours::whitesmoke.withAlpha (0.9f));
    g.setFont (11.0f);
    g.drawFittedText ("Key", keyBounds.toNearestInt(), juce::Justification::centredLeft, 1);
    g.drawFittedText ("Drive", driveBounds.toNearestInt(), juce::Justification::centredLeft, 1);
}

void FilterResponseEditor::resized() {}

void FilterResponseEditor::mouseDown (const juce::MouseEvent& e)
{
    const auto pos = e.position;
    if (juce::Rectangle<float> (12.0f, 12.0f).withCentre (getMainHandlePosition()).contains (pos))
    {
        dragTarget = DragTarget::main;
        return;
    }

    if (getMiniControlBounds (true).contains (pos))
    {
        dragTarget = DragTarget::keyTrack;
        mouseDrag (e);
        return;
    }

    if (getMiniControlBounds (false).contains (pos))
    {
        dragTarget = DragTarget::drive;
        mouseDrag (e);
        return;
    }

    dragTarget = DragTarget::none;
}

void FilterResponseEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (dragTarget == DragTarget::none)
        return;

    if (dragTarget == DragTarget::main)
    {
        writeParameterRealValue (0, xToCutoff (e.position.x));
        writeParameterRealValue (1, yToResonance (e.position.y));
    }
    else if (dragTarget == DragTarget::keyTrack)
    {
        writeParameterRealValue (2, xToMiniValue (e.position.x, true));
    }
    else if (dragTarget == DragTarget::drive)
    {
        writeParameterRealValue (3, xToMiniValue (e.position.x, false));
    }

    refreshFromParameters();
    repaint();
}

void FilterResponseEditor::mouseUp (const juce::MouseEvent&)
{
    dragTarget = DragTarget::none;
}

void FilterResponseEditor::timerCallback()
{
    const auto previous = values;
    refreshFromParameters();
    if (spectrumBins != nullptr || previous != values)
        repaint();
}

void FilterResponseEditor::setSpectrumData (const std::array<std::atomic<float>, 128>* bins,
                                            const std::atomic<float>* sampleRateHz)
{
    spectrumBins = bins;
    spectrumSampleRate = sampleRateHz;
}

void FilterResponseEditor::refreshFromParameters()
{
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (const auto* raw = apvts.getRawParameterValue (ids[i]))
            values[i] = raw->load();
    }
}

void FilterResponseEditor::writeParameterRealValue (int index, float realValue)
{
    if (auto* p = apvts.getParameter (ids[static_cast<size_t> (index)]))
        p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 (realValue));
}

juce::Rectangle<float> FilterResponseEditor::getPlotBounds() const
{
    auto r = getLocalBounds().toFloat().reduced (4.0f);
    return r.removeFromTop (r.getHeight() - 24.0f);
}

juce::Rectangle<float> FilterResponseEditor::getMiniControlBounds (bool forKeyTrack) const
{
    auto r = getLocalBounds().toFloat().reduced (4.0f);
    auto strip = r.removeFromBottom (20.0f);
    auto half = strip.removeFromLeft (strip.getWidth() * 0.5f).reduced (2.0f, 1.0f);
    auto other = strip.reduced (2.0f, 1.0f);
    return forKeyTrack ? half : other;
}

juce::Point<float> FilterResponseEditor::getMainHandlePosition() const
{
    const auto plot = getPlotBounds();
    const auto nx = std::log10 (values[0] / kCutoffMin) / std::log10 (kCutoffMax / kCutoffMin);
    const auto ny = 1.0f - values[1];
    return { plot.getX() + nx * plot.getWidth(), plot.getY() + ny * plot.getHeight() };
}

float FilterResponseEditor::xToCutoff (float x) const
{
    const auto plot = getPlotBounds();
    const auto nx = juce::jlimit (0.0f, 1.0f, (x - plot.getX()) / juce::jmax (1.0f, plot.getWidth()));
    const auto ratio = kCutoffMax / kCutoffMin;
    return kCutoffMin * std::pow (ratio, nx);
}

float FilterResponseEditor::yToResonance (float y) const
{
    const auto plot = getPlotBounds();
    const auto ny = juce::jlimit (0.0f, 1.0f, (y - plot.getY()) / juce::jmax (1.0f, plot.getHeight()));
    return juce::jmap (1.0f - ny, kResMin, kResMax);
}

float FilterResponseEditor::xToMiniValue (float x, bool forKeyTrack) const
{
    const auto b = getMiniControlBounds (forKeyTrack);
    const auto nx = juce::jlimit (0.0f, 1.0f, (x - b.getX()) / juce::jmax (1.0f, b.getWidth()));
    return forKeyTrack ? juce::jmap (nx, kKeyMin, kKeyMax) : juce::jmap (nx, kDriveMin, kDriveMax);
}
} // namespace UI
