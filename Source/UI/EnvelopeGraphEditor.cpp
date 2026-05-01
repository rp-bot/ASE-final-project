#include "EnvelopeGraphEditor.h"
#include <cmath>

namespace UI
{
namespace
{
constexpr float kAttackMin = 0.005f;
constexpr float kAttackMax = 10.0f;
constexpr float kDecayMin = 0.005f;
constexpr float kDecayMax = 10.0f;
constexpr float kReleaseMin = 0.005f;
constexpr float kReleaseMax = 10.0f;
constexpr float kTimeAxisExponent = 0.35f; // Expands short times for clearer visual scaling.

float timeToPercent (float value, float minValue, float maxValue)
{
    const auto linear = juce::jlimit (0.0f, 1.0f, (value - minValue) / juce::jmax (0.0001f, maxValue - minValue));
    return std::pow (linear, kTimeAxisExponent);
}

float percentToTime (float percent, float minValue, float maxValue)
{
    const auto linear = std::pow (juce::jlimit (0.0f, 1.0f, percent), 1.0f / kTimeAxisExponent);
    return juce::jmap (linear, minValue, maxValue);
}
} // namespace

EnvelopeGraphEditor::EnvelopeGraphEditor (juce::AudioProcessorValueTreeState& state,
                                          std::array<juce::String, 6> parameterIds,
                                          juce::Colour accentColour)
    : apvts (state), ids (std::move (parameterIds)), accent (accentColour)
{
    refreshFromParameters();
    startTimerHz (30);
}

void EnvelopeGraphEditor::paint (juce::Graphics& g)
{
    const auto plot = getPlotBounds();
    g.setColour (juce::Colours::black.withAlpha (0.25f));
    g.fillRoundedRectangle (plot, 4.0f);
    g.setColour (accent.withAlpha (0.55f));
    g.drawRoundedRectangle (plot, 4.0f, 1.0f);

    g.setColour (accent.withAlpha (0.2f));
    const auto midY = juce::jmap (0.5f, 1.0f, 0.0f, plot.getY(), plot.getBottom());
    g.drawHorizontalLine (juce::roundToInt (midY), plot.getX(), plot.getRight());

    const auto start = juce::Point<float> (plot.getX(), plot.getBottom());
    const auto attack = getAttackPoint();
    const auto decay = getDecayPoint();
    const auto release = getReleasePoint();
    const auto sustainEndX = juce::jlimit (decay.x + 8.0f, release.x - 8.0f,
                                           juce::jmin (release.x - 8.0f, decay.x + plot.getWidth() * 0.22f));
    const auto sustain = juce::Point<float> (sustainEndX, decay.y);
    const auto endPoint = juce::Point<float> (plot.getRight(), plot.getBottom());

    juce::Path envPath;
    envPath.startNewSubPath (start);
    envPath.lineTo (attack);
    envPath.lineTo (decay);
    envPath.lineTo (sustain);
    envPath.lineTo (release);
    envPath.lineTo (endPoint);
    g.setColour (accent.brighter (0.2f));
    g.strokePath (envPath, juce::PathStrokeType (1.6f));

    g.setColour (accent.withAlpha (0.9f));
    g.fillEllipse (juce::Rectangle<float> (9.0f, 9.0f).withCentre (attack));
    g.fillEllipse (juce::Rectangle<float> (9.0f, 9.0f).withCentre (decay));
    g.fillEllipse (juce::Rectangle<float> (9.0f, 9.0f).withCentre (release));

    g.setColour (juce::Colours::whitesmoke.withAlpha (0.9f));
    g.setFont (11.0f);
    const juce::String overlay = "Lvl " + juce::String (values[4], 2) + "  Vel " + juce::String (values[5], 2);
    g.drawFittedText (overlay, getLocalBounds().removeFromBottom (16), juce::Justification::centredRight, 1);
}

void EnvelopeGraphEditor::resized() {}

void EnvelopeGraphEditor::mouseDown (const juce::MouseEvent& e)
{
    const auto p = e.position;
    if (juce::Rectangle<float> (12.0f, 12.0f).withCentre (getAttackPoint()).contains (p))
    {
        dragTarget = DragTarget::attackNode;
        return;
    }

    if (juce::Rectangle<float> (12.0f, 12.0f).withCentre (getDecayPoint()).contains (p))
    {
        dragTarget = DragTarget::decayNode;
        return;
    }

    if (juce::Rectangle<float> (12.0f, 12.0f).withCentre (getReleasePoint()).contains (p))
    {
        dragTarget = DragTarget::releaseNode;
        return;
    }

    const auto plot = getPlotBounds();
    const auto sustainY = getDecayPoint().y;
    const juce::Rectangle<float> sustainHit (plot.getX() + plot.getWidth() * 0.4f, sustainY - 8.0f, plot.getWidth() * 0.3f, 16.0f);
    if (sustainHit.contains (p))
    {
        dragTarget = DragTarget::sustainSegment;
        mouseDrag (e);
        return;
    }

    dragTarget = DragTarget::none;
}

void EnvelopeGraphEditor::mouseDrag (const juce::MouseEvent& e)
{
    if (dragTarget == DragTarget::none)
        return;

    if (dragTarget == DragTarget::attackNode)
    {
        const auto plot = getPlotBounds();
        const auto attackMinX = plot.getX() + 8.0f;
        const auto attackMaxX = plot.getX() + plot.getWidth() * 0.34f;
        const auto newAttackTime =
            xToTimeSecondsInRegion (e.position.x, attackMinX, attackMaxX, kAttackMin, kAttackMax);
        writeParameterRealValue (0, newAttackTime);

        // If attack moves past the current decay point, push decay along with it so
        // the envelope stage order remains intuitive during drags.
        const auto decayMinX = plot.getX() + plot.getWidth() * 0.25f;
        const auto decayMaxX = plot.getX() + plot.getWidth() * 0.7f;
        const auto attackPointX = juce::jmap (newAttackTime, kAttackMin, kAttackMax, attackMinX, attackMaxX);
        const auto decayPointX = getDecayPoint().x;
        if (attackPointX + 8.0f > decayPointX)
        {
            const auto pushedDecayX = juce::jlimit (decayMinX, decayMaxX, attackPointX + 8.0f);
            const auto pushedDecayTime =
                xToTimeSecondsInRegion (pushedDecayX, decayMinX, decayMaxX, kDecayMin, kDecayMax);
            writeParameterRealValue (1, pushedDecayTime);
        }
    }
    else if (dragTarget == DragTarget::decayNode)
    {
        const auto plot = getPlotBounds();
        const auto minX = plot.getX() + plot.getWidth() * 0.25f;
        const auto maxX = plot.getX() + plot.getWidth() * 0.7f;
        const auto attackX = getAttackPoint().x + 8.0f;
        const auto clampedX = juce::jlimit (minX, maxX, juce::jmax (attackX, e.position.x));
        writeParameterRealValue (1, xToTimeSecondsInRegion (clampedX, minX, maxX, kDecayMin, kDecayMax));
        writeParameterRealValue (2, yToSustainValue (e.position.y));
    }
    else if (dragTarget == DragTarget::sustainSegment)
    {
        writeParameterRealValue (2, yToSustainValue (e.position.y));
    }
    else if (dragTarget == DragTarget::releaseNode)
    {
        const auto plot = getPlotBounds();
        const auto releaseMinX = juce::jmin (plot.getRight() - 16.0f, getDecayPoint().x + plot.getWidth() * 0.12f);
        const auto releaseMaxX = plot.getRight() - 8.0f;
        const auto newReleaseTime =
            xToTimeSecondsInRegion (e.position.x, releaseMinX, releaseMaxX, kReleaseMin, kReleaseMax);
        writeParameterRealValue (3, newReleaseTime);

        // Mirror attack->decay behavior: if release is pulled left into the sustain
        // segment, push decay/sustain left so stage ordering remains stable.
        const auto newReleaseX = juce::jmap (newReleaseTime, kReleaseMin, kReleaseMax, releaseMinX, releaseMaxX);
        const auto decayPoint = getDecayPoint();
        const auto sustainEndX = juce::jmin (plot.getRight() - 28.0f, decayPoint.x + plot.getWidth() * 0.27f);
        if (newReleaseX < sustainEndX + 8.0f)
        {
            const auto decayMinX = plot.getX() + plot.getWidth() * 0.25f;
            const auto decayMaxX = plot.getX() + plot.getWidth() * 0.7f;
            const auto minDecayFromAttack = getAttackPoint().x + 8.0f;
            const auto desiredDecayX = (newReleaseX - 8.0f) - plot.getWidth() * 0.27f;
            const auto pushedDecayX = juce::jlimit (juce::jmax (decayMinX, minDecayFromAttack), decayMaxX, desiredDecayX);
            const auto pushedDecayTime =
                xToTimeSecondsInRegion (pushedDecayX, decayMinX, decayMaxX, kDecayMin, kDecayMax);
            writeParameterRealValue (1, pushedDecayTime);
        }
    }

    refreshFromParameters();
    repaint();
}

void EnvelopeGraphEditor::mouseUp (const juce::MouseEvent&)
{
    dragTarget = DragTarget::none;
}

void EnvelopeGraphEditor::timerCallback()
{
    const auto previous = values;
    refreshFromParameters();
    if (previous != values)
        repaint();
}

void EnvelopeGraphEditor::refreshFromParameters()
{
    for (size_t i = 0; i < ids.size(); ++i)
    {
        if (const auto* raw = apvts.getRawParameterValue (ids[i]))
            values[i] = raw->load();
    }
}

void EnvelopeGraphEditor::writeParameterRealValue (int index, float realValue)
{
    if (auto* p = apvts.getParameter (ids[static_cast<size_t> (index)]))
        p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 (realValue));
}

juce::Rectangle<float> EnvelopeGraphEditor::getPlotBounds() const
{
    return getLocalBounds().toFloat().reduced (4.0f, 4.0f).withTrimmedBottom (16.0f);
}

juce::Point<float> EnvelopeGraphEditor::getAttackPoint() const
{
    const auto plot = getPlotBounds();
    const auto x = juce::jmap (timeToPercent (values[0], kAttackMin, kAttackMax),
                               0.0f, 1.0f,
                               plot.getX() + 8.0f, plot.getX() + plot.getWidth() * 0.34f);
    return { x, plot.getY() + 4.0f };
}

juce::Point<float> EnvelopeGraphEditor::getDecayPoint() const
{
    const auto plot = getPlotBounds();
    const auto x = juce::jmap (timeToPercent (values[1], kDecayMin, kDecayMax),
                               0.0f, 1.0f,
                               plot.getX() + plot.getWidth() * 0.25f, plot.getX() + plot.getWidth() * 0.7f);
    const auto y = juce::jmap (values[2], 1.0f, 0.0f, plot.getY() + 4.0f, plot.getBottom() - 4.0f);
    return { x, y };
}

juce::Point<float> EnvelopeGraphEditor::getReleasePoint() const
{
    const auto plot = getPlotBounds();
    const auto minX = juce::jmin (plot.getRight() - 16.0f, getDecayPoint().x + plot.getWidth() * 0.12f);
    const auto maxX = plot.getRight() - 8.0f;
    const auto x = juce::jmap (timeToPercent (values[3], kReleaseMin, kReleaseMax), 0.0f, 1.0f, minX, maxX);
    return { x, plot.getBottom() };
}

float EnvelopeGraphEditor::xToTimeSecondsInRegion (float x, float minX, float maxX, float minTime, float maxTime) const
{
    const auto nx = juce::jlimit (0.0f, 1.0f, (x - minX) / juce::jmax (1.0f, maxX - minX));
    return percentToTime (nx, minTime, maxTime);
}

float EnvelopeGraphEditor::yToSustainValue (float y) const
{
    const auto plot = getPlotBounds();
    const auto ny = juce::jlimit (0.0f, 1.0f, (y - plot.getY()) / juce::jmax (1.0f, plot.getHeight()));
    return juce::jmap (1.0f - ny, 0.0f, 1.0f);
}
} // namespace UI
