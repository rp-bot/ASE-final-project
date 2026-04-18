#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace UI {

/** 2D pad: horizontal = first slider [0,1], vertical = second (bottom = 0, top = 1). Mouse wheel can control an optional height slider. */
class CursorJoystick2D : public juce::Component,
                          private juce::Slider::Listener
{
public:
    CursorJoystick2D();
    ~CursorJoystick2D() override;

    void setXYSliders (juce::Slider* xSlider, juce::Slider* ySlider) noexcept;
    void setScrollSlider (juce::Slider* scrollSlider) noexcept;

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

private:
    void sliderValueChanged (juce::Slider*) override;
    void updateFromLocalPosition (juce::Point<float> localPos);

    juce::Slider* xSlider_ { nullptr };
    juce::Slider* ySlider_ { nullptr };
    juce::Slider* scrollSlider_ { nullptr };
};

} // namespace UI
