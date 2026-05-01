#pragma once

#include <array>
#include <cmath>
#include <functional>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace UI
{

/** Parsed typed value from a master control text field (Hz, ms, %, etc.). */
inline float parseMasterControlTextValue (juce::String text, bool preferMilliseconds)
{
    text = text.trim();
    if (text.endsWithIgnoreCase ("khz"))
        return static_cast<float> (text.dropLastCharacters (3).trimEnd().getDoubleValue() * 1000.0);
    if (text.endsWithIgnoreCase ("hz"))
        return static_cast<float> (text.dropLastCharacters (2).trimEnd().getDoubleValue());
    if (text.endsWithIgnoreCase ("ms"))
        return static_cast<float> (text.dropLastCharacters (2).trimEnd().getDoubleValue() / 1000.0);
    if (text.endsWithIgnoreCase ("s"))
        return static_cast<float> (text.dropLastCharacters (1).trimEnd().getDoubleValue());
    if (text.endsWith ("%"))
        return static_cast<float> (text.dropLastCharacters (1).trimEnd().getDoubleValue() / 100.0);
    if (text.endsWithIgnoreCase ("k"))
        return static_cast<float> (text.dropLastCharacters (1).getDoubleValue() * 1000.0);
    const auto raw = static_cast<float> (text.getDoubleValue());
    return preferMilliseconds ? raw / 1000.0f : raw;
}

/** Text editor that supports vertical drag to change numeric parameter values. */
class MasterDragValueEditor : public juce::TextEditor
{
public:
    void configure (float minValue, float maxValue,
                    std::function<float()> valueGetter,
                    std::function<void(float)> valueSetter,
                    bool useLogDrag = false);

    void mouseDown (const juce::MouseEvent& event) override;
    void mouseDrag (const juce::MouseEvent& event) override;
    void mouseUp (const juce::MouseEvent& event) override;
    bool isDraggingValue() const noexcept { return didDrag; }

private:
    float minV { 0.0f };
    float maxV { 1.0f };
    float dragStartValue { 0.0f };
    int dragStartY { 0 };
    bool didDrag { false };
    bool logDrag { false };
    std::function<float()> getter;
    std::function<void(float)> setter;
};

inline void MasterDragValueEditor::configure (float minValue, float maxValue,
                                              std::function<float()> valueGetter,
                                              std::function<void(float)> valueSetter,
                                              bool useLogDrag)
{
    minV = minValue;
    maxV = maxValue;
    getter = std::move (valueGetter);
    setter = std::move (valueSetter);
    logDrag = useLogDrag;
}

inline void MasterDragValueEditor::mouseDown (const juce::MouseEvent& event)
{
    didDrag = false;
    dragStartY = event.getPosition().y;
    dragStartValue = getter != nullptr ? getter() : 0.0f;
    TextEditor::mouseDown (event);
}

inline void MasterDragValueEditor::mouseDrag (const juce::MouseEvent& event)
{
    if (setter == nullptr)
        return;

    const auto deltaY = static_cast<float> (dragStartY - event.getPosition().y);
    if (std::abs (deltaY) < 2.0f)
        return;

    didDrag = true;
    float newValue = dragStartValue;
    if (logDrag && minV > 0.0f && maxV > minV)
    {
        const auto ratio = maxV / minV;
        const auto startNorm = std::log (juce::jmax (dragStartValue, minV) / minV) / std::log (ratio);
        const auto normDelta = deltaY / 180.0f;
        const auto nextNorm = juce::jlimit (0.0f, 1.0f, startNorm + normDelta);
        newValue = minV * std::pow (ratio, nextNorm);
    }
    else
    {
        const auto sensitivity = juce::jmax (0.0001f, (maxV - minV) / 160.0f);
        newValue = juce::jlimit (minV, maxV, dragStartValue + deltaY * sensitivity);
    }
    setter (newValue);
}

inline void MasterDragValueEditor::mouseUp (const juce::MouseEvent& event)
{
    if (! didDrag)
        TextEditor::mouseUp (event);
    didDrag = false;
}

/** Suppresses mouse-hover highlight on toggle buttons (click-down feedback preserved). */
class NoHoverToggleLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool /*shouldDrawButtonAsHighlighted*/,
                               bool shouldDrawButtonAsDown) override
    {
        juce::LookAndFeel_V4::drawButtonBackground (g, button, backgroundColour,
                                                    false, shouldDrawButtonAsDown);
    }
};

inline void writeParameterNormalised (juce::AudioProcessorValueTreeState& apvts,
                                      const juce::String& parameterId,
                                      float realValue)
{
    if (auto* p = apvts.getParameter (parameterId))
        p->setValueNotifyingHost (p->getNormalisableRange().convertTo0to1 (realValue));
}

inline juce::String formatMasterParameterDisplayValue (const juce::String& name, float value)
{
    if (name == "Cutoff")
        return value >= 1000.0f ? juce::String (value / 1000.0f, 2) + " kHz" : juce::String (value, 0) + " Hz";
    if (name == "Res" || name == "Key" || name == "Drive")
        return juce::String (juce::roundToInt (value * 100.0f)) + " %";
    if (name == "Attack" || name == "Decay" || name == "Release")
    {
        if (value < 1.0f)
            return juce::String (juce::roundToInt (value * 1000.0f)) + " ms";
        return juce::String (value, 2) + " s";
    }
    return juce::String (value, 2);
}

/** Per-oscillator accent palette; keep in sync with `moduleColours` in SynthEditorWorkspace.cpp. */
inline const std::array<juce::Colour, 8>& masterSectionOscColours()
{
    static const std::array<juce::Colour, 8> colours {
        juce::Colour::fromRGB (231, 76, 60),
        juce::Colour::fromRGB (230, 126, 34),
        juce::Colour::fromRGB (241, 196, 15),
        juce::Colour::fromRGB (46, 204, 113),
        juce::Colour::fromRGB (26, 188, 156),
        juce::Colour::fromRGB (52, 152, 219),
        juce::Colour::fromRGB (155, 89, 182),
        juce::Colour::fromRGB (236, 112, 173)
    };
    return colours;
}

} // namespace UI
