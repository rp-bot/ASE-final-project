#include "CursorJoystick2D.h"
#include "UI/Common/SynthLookAndFeel.h"

namespace UI {

CursorJoystick2D::CursorJoystick2D() = default;

CursorJoystick2D::~CursorJoystick2D()
{
    if (xSlider_ != nullptr)
        xSlider_->removeListener (this);
    if (ySlider_ != nullptr)
        ySlider_->removeListener (this);
}

void CursorJoystick2D::setXYSliders (juce::Slider* xSlider, juce::Slider* ySlider) noexcept
{
    if (xSlider_ != nullptr)
        xSlider_->removeListener (this);
    if (ySlider_ != nullptr)
        ySlider_->removeListener (this);

    xSlider_ = xSlider;
    ySlider_ = ySlider;

    if (xSlider_ != nullptr)
        xSlider_->addListener (this);
    if (ySlider_ != nullptr)
        ySlider_->addListener (this);
}

void CursorJoystick2D::setScrollSlider (juce::Slider* scrollSlider) noexcept
{
    scrollSlider_ = scrollSlider;
}

void CursorJoystick2D::sliderValueChanged (juce::Slider*)
{
    repaint();
}

void CursorJoystick2D::paint (juce::Graphics& g)
{
    auto pad = getLocalBounds().toFloat().reduced (3.0f);
    g.setColour (SynthLookAndFeel::panelSurface());
    g.fillRoundedRectangle (pad, 4.0f);
    g.setColour (SynthLookAndFeel::panelBorder());
    g.drawRoundedRectangle (pad, 4.0f, 1.0f);

    // Subtle grid to help precise placement.
    {
        const int gridDivs = 4; // draws 3 interior lines each direction
        g.setColour (SynthLookAndFeel::panelBorder().withAlpha (0.5f));
        for (int i = 1; i < gridDivs; ++i)
        {
            const float t = static_cast<float> (i) / static_cast<float> (gridDivs);
            const float x = pad.getX() + t * pad.getWidth();
            const float y = pad.getY() + t * pad.getHeight();
            g.drawLine (x, pad.getY(), x, pad.getBottom(), 1.0f);
            g.drawLine (pad.getX(), y, pad.getRight(), y, 1.0f);
        }
    }

    const float x = (xSlider_ != nullptr) ? static_cast<float> (xSlider_->getValue()) : 0.5f;
    const float y = (ySlider_ != nullptr) ? static_cast<float> (ySlider_->getValue()) : 0.5f;

    const float px = pad.getX() + x * pad.getWidth();
    const float py = pad.getBottom() - y * pad.getHeight();

    g.setColour (SynthLookAndFeel::arcTrack());
    g.drawLine (pad.getX(), py, pad.getRight(), py, 1.0f);
    g.drawLine (px, pad.getY(), px, pad.getBottom(), 1.0f);

    const float r = 6.0f;
    g.setColour (SynthLookAndFeel::teal());
    g.fillEllipse (px - r, py - r, r * 2.0f, r * 2.0f);
    g.setColour (SynthLookAndFeel::panelBorder());
    g.drawEllipse (px - r, py - r, r * 2.0f, r * 2.0f, 1.2f);
}

void CursorJoystick2D::mouseDown (const juce::MouseEvent& e) { updateFromLocalPosition (e.position); }

void CursorJoystick2D::mouseDrag (const juce::MouseEvent& e) { updateFromLocalPosition (e.position); }

void CursorJoystick2D::mouseWheelMove (const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    if (scrollSlider_ == nullptr)
        return;

    const double minValue = scrollSlider_->getMinimum();
    const double maxValue = scrollSlider_->getMaximum();
    const double range = maxValue - minValue;

    if (range <= 0.0)
        return;

    // Scale wheel delta to a practical normalized step amount.
    const double delta = static_cast<double> (wheel.deltaY) * 0.08 * range;
    const double newValue = juce::jlimit (minValue, maxValue, scrollSlider_->getValue() + delta);
    scrollSlider_->setValue (newValue, juce::sendNotificationSync);
}

void CursorJoystick2D::updateFromLocalPosition (juce::Point<float> localPos)
{
    if (xSlider_ == nullptr || ySlider_ == nullptr)
        return;

    auto pad = getLocalBounds().toFloat().reduced (3.0f);
    if (pad.getWidth() <= 0.0f || pad.getHeight() <= 0.0f)
        return;

    float nx = (localPos.x - pad.getX()) / pad.getWidth();
    float ny = (pad.getBottom() - localPos.y) / pad.getHeight();
    nx = juce::jlimit (0.0f, 1.0f, nx);
    ny = juce::jlimit (0.0f, 1.0f, ny);

    xSlider_->setValue (static_cast<double> (nx), juce::sendNotificationSync);
    ySlider_->setValue (static_cast<double> (ny), juce::sendNotificationSync);
    repaint();
}

} // namespace UI
