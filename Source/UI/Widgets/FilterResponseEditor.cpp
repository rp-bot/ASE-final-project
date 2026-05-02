#include "FilterResponseEditor.h"
#include "UI/Common/SynthLookAndFeel.h"

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

    FilterResponseEditor::FilterResponseEditor(juce::AudioProcessorValueTreeState& state,
        std::array<juce::String, 4> parameterIds,
        juce::Colour accentColour)
        : apvts(state), ids(std::move(parameterIds)), accent(accentColour)
    {
        refreshFromParameters();
        startTimerHz(30);
    }

    void FilterResponseEditor::paint(juce::Graphics& g)
    {
        const auto plot = getPlotBounds();
        g.setColour(SynthLookAndFeel::panelSurface());
        g.fillRoundedRectangle(plot, 4.0f);
        g.setColour(SynthLookAndFeel::panelBorder());
        g.drawRoundedRectangle(plot, 4.0f, 1.0f);

        // Frequency decade guides.
        constexpr std::array<float, 4> guides{ 100.0f, 1000.0f, 10000.0f, 20000.0f };
        g.setColour(accent.withAlpha(0.2f));
        for (const auto hz : guides)
        {
            const auto nx = std::log10(hz / kCutoffMin) / std::log10(kCutoffMax / kCutoffMin);
            const auto x = plot.getX() + (nx * plot.getWidth());
            g.drawVerticalLine(juce::roundToInt(x), plot.getY(), plot.getBottom());
        }
        g.setFont(juce::Font("Helvetica Neue", 10.0f, juce::Font::plain));
        g.setColour(SynthLookAndFeel::textDim());
        for (const auto hz : guides)
        {
            const auto nx = std::log10(hz / kCutoffMin) / std::log10(kCutoffMax / kCutoffMin);
            const auto x = plot.getX() + (nx * plot.getWidth());
            juce::Rectangle<int> labelArea(juce::roundToInt(x - 22.0f),
                juce::roundToInt(plot.getBottom() - 14.0f),
                44, 12);
            const auto label = hz >= 1000.0f ? juce::String(hz / 1000.0f, 0) + "k" : juce::String(hz, 0);
            g.drawFittedText(label, labelArea, juce::Justification::centred, 1);
        }

        const auto cutoffNorm = std::log10(values[0] / kCutoffMin) / std::log10(kCutoffMax / kCutoffMin);
        // Bump height follows raw resonance 0→1 so at visual minimum dot (50%, res=0) peak is gone.
        const auto resonanceBumpScale = values[1];
        const auto driveBoost = values[3] * 8.0f;
        const auto slopeDepth = 52.0f + values[2] * 28.0f;

        juce::Path response;
        constexpr int samples = 128;
        for (int i = 0; i < samples; ++i)
        {
            const auto nx = static_cast<float>(i) / static_cast<float>(samples - 1);
            const auto x = plot.getX() + nx * plot.getWidth();

            const auto dx = nx - cutoffNorm;
            const auto postCut = juce::jmax(0.0f, dx);
            const auto lowPassShape = -(std::pow(postCut * 1.85f, 1.25f)) * slopeDepth;
            const auto resonancePeak = std::exp(-120.0f * dx * dx) * (resonanceBumpScale * 12.0f);
            const auto db = juce::jlimit(-24.0f, 12.0f, lowPassShape + resonancePeak + driveBoost);
            const auto y = juce::jmap(db, 12.0f, -24.0f, plot.getY(), plot.getBottom());

            if (i == 0)
                response.startNewSubPath(x, y);
            else
                response.lineTo(x, y);
        }

        g.setColour(accent.darker(0.55f));
        g.strokePath(response, juce::PathStrokeType(1.5f));

        if (spectrumBins != nullptr)
        {
            juce::Path spectrumPath;
            for (size_t i = 0; i < spectrumBins->size(); ++i)
            {
                const auto t = static_cast<float>(i) / static_cast<float>(spectrumBins->size() - 1);
                const auto x = plot.getX() + t * plot.getWidth();
                const auto mag = (*spectrumBins)[i].load(std::memory_order_relaxed);
                const auto y = juce::jmap(mag, 0.0f, 1.0f, plot.getBottom(), plot.getY());
                if (i == 0)
                    spectrumPath.startNewSubPath(x, y);
                else
                    spectrumPath.lineTo(x, y);
            }

            g.setColour(SynthLookAndFeel::teal().withAlpha(0.5f));
            g.strokePath(spectrumPath, juce::PathStrokeType(1.1f));
        }

        const auto main = getMainHandlePosition();
        g.setColour(accent.darker(0.35f));
        g.fillEllipse(juce::Rectangle<float>(10.0f, 10.0f).withCentre(main));
        g.setColour(SynthLookAndFeel::panelBorder());
        g.drawEllipse(juce::Rectangle<float>(10.0f, 10.0f).withCentre(main), 1.0f);
        g.setColour(SynthLookAndFeel::textPrimary());
        g.setFont(juce::Font("Helvetica Neue", 10.5f, juce::Font::plain));
        const auto cutoffLabel = values[0] >= 1000.0f
            ? juce::String(values[0] / 1000.0f, 2) + " kHz"
            : juce::String(values[0], 0) + " Hz";
        juce::Rectangle<int> cutoffArea(juce::roundToInt(main.x + 8.0f),
            juce::roundToInt(main.y - 16.0f),
            64, 14);
        g.drawFittedText(cutoffLabel, cutoffArea, juce::Justification::centredLeft, 1);

        const auto keyBounds = getMiniControlBounds(true);
        const auto driveBounds = getMiniControlBounds(false);
        const auto barColour = accent.darker(0.55f);

        // Dark track behind each mini control.
        g.setColour(SynthLookAndFeel::arcTrack());
        g.fillRoundedRectangle(keyBounds, 3.0f);
        g.fillRoundedRectangle(driveBounds, 3.0f);

        // Filled progress by percentage (0..1).
        auto fillByPercent = [&g, &barColour] (juce::Rectangle<float> bounds, float percent)
        {
            const float p = juce::jlimit(0.0f, 1.0f, percent);
            const float fillW = bounds.getWidth() * p;
            if (fillW <= 0.5f)
                return;

            auto fill = bounds.withWidth(fillW);
            g.setColour(barColour);
            g.fillRoundedRectangle(fill, 3.0f);
        };

        fillByPercent(keyBounds, values[2]);
        fillByPercent(driveBounds, values[3]);

        g.setColour(SynthLookAndFeel::panelBorder());
        g.drawRoundedRectangle(keyBounds, 3.0f, 1.0f);
        g.drawRoundedRectangle(driveBounds, 3.0f, 1.0f);

        g.setColour(SynthLookAndFeel::textPrimary());
        g.setFont(juce::Font("Helvetica Neue", 11.0f, juce::Font::plain));
        g.drawFittedText("Key", keyBounds.toNearestInt(), juce::Justification::centredLeft, 1);
        g.drawFittedText("Drive", driveBounds.toNearestInt(), juce::Justification::centredLeft, 1);
    }

    void FilterResponseEditor::resized() {}

    void FilterResponseEditor::mouseDown(const juce::MouseEvent& e)
    {
        const auto pos = e.position;
        if (juce::Rectangle<float>(12.0f, 12.0f).withCentre(getMainHandlePosition()).contains(pos))
        {
            dragTarget = DragTarget::main;
            dragStartPos = pos;
            dragStartCutoff = values[0];
            dragStartResonance = juce::jmap(values[1], 0.0f, 1.0f, 0.5f, 1.0f);
            return;
        }

        if (getMiniControlBounds(true).contains(pos))
        {
            dragTarget = DragTarget::keyTrack;
            mouseDrag(e);
            return;
        }

        if (getMiniControlBounds(false).contains(pos))
        {
            dragTarget = DragTarget::drive;
            mouseDrag(e);
            return;
        }

        dragTarget = DragTarget::none;
    }

    void FilterResponseEditor::mouseDrag(const juce::MouseEvent& e)
    {
        if (dragTarget == DragTarget::none)
            return;

        if (dragTarget == DragTarget::main)
        {
            const auto plot = getPlotBounds();
            const auto dxNorm = (e.position.x - dragStartPos.x) / juce::jmax(1.0f, plot.getWidth());
            const auto dyNorm = (dragStartPos.y - e.position.y) / juce::jmax(1.0f, plot.getHeight());

            // Cutoff drag is logarithmic for even control across octaves.
            const auto ratio = kCutoffMax / kCutoffMin;
            const auto startCutoffNorm = std::log10(dragStartCutoff / kCutoffMin) / std::log10(ratio);
            const auto nextCutoffNorm = juce::jlimit(0.0f, 1.0f, startCutoffNorm + dxNorm);
            writeParameterRealValue(0, kCutoffMin * std::pow(ratio, nextCutoffNorm));

            // Frequency gain / resonance drag is relative and smoothed for precision.
            const auto nextVisualRes = juce::jlimit(0.5f, 1.0f, dragStartResonance + (dyNorm * 0.6f));
            writeParameterRealValue(1, juce::jmap(nextVisualRes, 0.5f, 1.0f, 0.0f, 1.0f));
        } else if (dragTarget == DragTarget::keyTrack)
        {
            writeParameterRealValue(2, xToMiniValue(e.position.x, true));
        } else if (dragTarget == DragTarget::drive)
        {
            writeParameterRealValue(3, xToMiniValue(e.position.x, false));
        }

        refreshFromParameters();
        repaint();
    }

    void FilterResponseEditor::mouseUp(const juce::MouseEvent&)
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

    void FilterResponseEditor::setSpectrumData(const std::array<std::atomic<float>, 128>* bins,
        const std::atomic<float>* sampleRateHz)
    {
        spectrumBins = bins;
        spectrumSampleRate = sampleRateHz;
    }

    void FilterResponseEditor::refreshFromParameters()
    {
        for (size_t i = 0; i < ids.size(); ++i)
        {
            if (const auto* raw = apvts.getRawParameterValue(ids[i]))
                values[i] = raw->load();
        }
    }

    void FilterResponseEditor::writeParameterRealValue(int index, float realValue)
    {
        if (auto* p = apvts.getParameter(ids[static_cast<size_t>(index)]))
            p->setValueNotifyingHost(p->getNormalisableRange().convertTo0to1(realValue));
    }

    juce::Rectangle<float> FilterResponseEditor::getPlotBounds() const
    {
        auto r = getLocalBounds().toFloat().reduced(4.0f);
        return r.removeFromTop(r.getHeight() - 24.0f);
    }

    juce::Rectangle<float> FilterResponseEditor::getMiniControlBounds(bool forKeyTrack) const
    {
        auto r = getLocalBounds().toFloat().reduced(4.0f);
        auto strip = r.removeFromBottom(20.0f);
        auto half = strip.removeFromLeft(strip.getWidth() * 0.5f).reduced(2.0f, 1.0f);
        auto other = strip.reduced(2.0f, 1.0f);
        return forKeyTrack ? half : other;
    }

    juce::Point<float> FilterResponseEditor::getMainHandlePosition() const
    {
        const auto plot = getPlotBounds();
        const auto nx = std::log10(values[0] / kCutoffMin) / std::log10(kCutoffMax / kCutoffMin);
        const auto visualQ = juce::jmap(values[1], 0.0f, 1.0f, 0.5f, 1.0f);
        const auto ny = 1.0f - visualQ;
        return { plot.getX() + nx * plot.getWidth(), plot.getY() + ny * plot.getHeight() };
    }

    float FilterResponseEditor::xToCutoff(float x) const
    {
        const auto plot = getPlotBounds();
        const auto nx = juce::jlimit(0.0f, 1.0f, (x - plot.getX()) / juce::jmax(1.0f, plot.getWidth()));
        const auto ratio = kCutoffMax / kCutoffMin;
        return kCutoffMin * std::pow(ratio, nx);
    }

    float FilterResponseEditor::yToResonance(float y) const
    {
        const auto plot = getPlotBounds();
        const auto ny = juce::jlimit(0.0f, 1.0f, (y - plot.getY()) / juce::jmax(1.0f, plot.getHeight()));
        return juce::jmap(1.0f - ny, 0.5f, kResMax);
    }

    float FilterResponseEditor::xToMiniValue(float x, bool forKeyTrack) const
    {
        const auto b = getMiniControlBounds(forKeyTrack);
        const auto nx = juce::jlimit(0.0f, 1.0f, (x - b.getX()) / juce::jmax(1.0f, b.getWidth()));
        return forKeyTrack ? juce::jmap(nx, kKeyMin, kKeyMax) : juce::jmap(nx, kDriveMin, kDriveMax);
    }
} // namespace UI
