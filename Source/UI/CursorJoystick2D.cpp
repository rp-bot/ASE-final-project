#include "CursorJoystick2D.h"

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

void CursorJoystick2D::sliderValueChanged (juce::Slider*)
{
    repaint();
}

void CursorJoystick2D::paint (juce::Graphics& g)
{
    auto pad = getLocalBounds().toFloat().reduced (3.0f);
    g.setColour (juce::Colours::white.withAlpha (0.06f));
    g.fillRoundedRectangle (pad, 4.0f);
    g.setColour (juce::Colours::lightgrey.withAlpha (0.5f));
    g.drawRoundedRectangle (pad, 4.0f, 1.0f);

    // Subtle grid to help precise placement.
    {
        const int gridDivs = 4; // draws 3 interior lines each direction
        g.setColour (juce::Colours::white.withAlpha (0.06f));
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

    g.setColour (juce::Colours::white.withAlpha (0.15f));
    g.drawLine (pad.getX(), py, pad.getRight(), py, 1.0f);
    g.drawLine (px, pad.getY(), px, pad.getBottom(), 1.0f);

    const float r = 6.0f;
    g.setColour (juce::Colour (0xff4a9eff));
    g.fillEllipse (px - r, py - r, r * 2.0f, r * 2.0f);
    g.setColour (juce::Colours::white.withAlpha (0.85f));
    g.drawEllipse (px - r, py - r, r * 2.0f, r * 2.0f, 1.2f);
}

void CursorJoystick2D::mouseDown (const juce::MouseEvent& e) { updateFromLocalPosition (e.position); }

void CursorJoystick2D::mouseDrag (const juce::MouseEvent& e) { updateFromLocalPosition (e.position); }

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
