#include "UI/Workspace/SynthEditorWorkspace.h"
#include "Audio/SynthEngine.h"
#include "Parameters/ParameterIDs.h"

namespace UI
{

SynthEditorWorkspace* GLViewportComponent::getWorkspace() const noexcept
{
    return dynamic_cast<SynthEditorWorkspace*> (getParentComponent());
}

void GLViewportComponent::mouseDown (const juce::MouseEvent& e)
{
    if (auto* w = getWorkspace())
        w->mouseDown (e.getEventRelativeTo (w));
}

void GLViewportComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (auto* w = getWorkspace())
        w->mouseDrag (e.getEventRelativeTo (w));
}

void GLViewportComponent::mouseUp (const juce::MouseEvent& e)
{
    if (auto* w = getWorkspace())
        w->mouseUp (e.getEventRelativeTo (w));
}

void GLViewportComponent::mouseExit (const juce::MouseEvent& e)
{
    juce::ignoreUnused (e);
    if (auto* w = getWorkspace())
        w->cancelViewportInteraction();
}

void GLViewportComponent::mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w)
{
    if (auto* ws = getWorkspace())
        ws->mouseWheelMove (e.getEventRelativeTo (ws), w);
}

//==============================================================================
SynthEditorWorkspace::SynthEditorWorkspace (VolumetricSynthAudioProcessor& processor)
    : processorRef (processor)
{
    auto& apvts = processorRef.getParameterManager().getAPVTS();
    const std::array<juce::Colour, 8> moduleColours {
        juce::Colour::fromRGB (231, 76, 60),
        juce::Colour::fromRGB (230, 126, 34),
        juce::Colour::fromRGB (241, 196, 15),
        juce::Colour::fromRGB (46, 204, 113),
        juce::Colour::fromRGB (64, 224, 208),
        juce::Colour::fromRGB (52, 152, 219),
        juce::Colour::fromRGB (155, 89, 182),
        juce::Colour::fromRGB (236, 112, 173)
    };

    for (int module = 0; module < modulesPerBank; ++module)
    {
        leftModules[static_cast<size_t> (module)] = std::make_unique<OscillatorModuleComponent> (
            apvts,
            module,
            "Osc " + juce::String (module + 1),
            moduleColours[static_cast<size_t> (module)]);
        addAndMakeVisible (*leftModules[static_cast<size_t> (module)]);

        rightModules[static_cast<size_t> (module)] = std::make_unique<OscillatorModuleComponent> (
            apvts,
            modulesPerBank + module,
            "Osc " + juce::String (modulesPerBank + module + 1),
            moduleColours[static_cast<size_t> (modulesPerBank + module)]);
        addAndMakeVisible (*rightModules[static_cast<size_t> (module)]);
    }

    centerPanel = std::make_unique<CenterControlPanel> (apvts);
    addAndMakeVisible (*centerPanel);

    topBar = std::make_unique<TopBar> (apvts, [this]()
                                         { processorRef.resetEngineHardOff(); });
    addAndMakeVisible (*topBar);

    ampSection = std::make_unique<AmpEnvelopeSection> (apvts);
    addAndMakeVisible (*ampSection);

    filterSection = std::make_unique<FilterSection> (apvts);
    filterSection->setSpectrumDataSource (&processorRef.getSpectrumData(),
                                         &processorRef.getSpectrumSampleRateHz());
    addAndMakeVisible (*filterSection);

    addAndMakeVisible (glViewport_);
    glContextHost_.setRenderer (&renderer3D_);
    glContextHost_.attachTo (glViewport_);

    {
        std::array<glm::vec4, 8> cornerColours {};
        for (size_t i = 0; i < cornerColours.size(); ++i)
        {
            const auto c = moduleColours[i];
            cornerColours[i] = { c.getFloatRed(), c.getFloatGreen(), c.getFloatBlue(), 1.0f };
        }
        renderer3D_.setCornerColours (cornerColours);
    }

    auto& apvtsCursor = processorRef.getParameterManager().getAPVTS();
    auto* pcx = apvtsCursor.getRawParameterValue (ParameterIDs::cursorX);
    auto* pcy = apvtsCursor.getRawParameterValue (ParameterIDs::cursorY);
    auto* pcz = apvtsCursor.getRawParameterValue (ParameterIDs::cursorZ);
    lastSyncedCursorGlobal_ = glm::vec3 (
        pcx != nullptr ? pcx->load() : 0.5f,
        pcy != nullptr ? pcy->load() : 0.5f,
        pcz != nullptr ? pcz->load() : 0.5f);

    renderer3D_.setCursorFromGlobalUnitPosition (lastSyncedCursorGlobal_);
    processorRef.setGuiCursorPosition (lastSyncedCursorGlobal_);
    processorRef.setGuiCubeRotation (renderer3D_.getCubeRotationQuat ());
}

SynthEditorWorkspace::~SynthEditorWorkspace()
{
    glContextHost_.detach();
}

void SynthEditorWorkspace::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SynthEditorWorkspace::resized()
{
    // Scale padding values down as the window shrinks toward its minimum size so
    // that knobs absorb less of the reduction and padding absorbs more of it.
    constexpr float kDefW = 1220.0f, kDefH = 880.0f;
    constexpr float kMinW = 1000.0f, kMinH = 640.0f;
    const float ws = juce::jlimit (0.0f, 1.0f, (static_cast<float> (getWidth())  - kMinW) / (kDefW - kMinW));
    const float hs = juce::jlimit (0.0f, 1.0f, (static_cast<float> (getHeight()) - kMinH) / (kDefH - kMinH));

    auto lerpPad = [] (float t, int lo, int hi) -> int
    {
        return lo + juce::roundToInt (t * static_cast<float> (hi - lo));
    };

    const int outerPadH    = lerpPad (ws, 2, 8);   // horizontal outer padding (was fixed 8)
    const int outerPadV    = lerpPad (hs, 2, 8);   // vertical outer padding   (was fixed 8)
    const int topGap       = lerpPad (hs, 2, 8);   // gap below top bar        (was fixed 8)
    const int colGap       = lerpPad (ws, 2, 8);   // column gap               (was fixed 8)
    const int bankPadH     = lerpPad (ws, 2, 6);   // bank horizontal padding  (was fixed 6)
    const int bankPadV     = lerpPad (hs, 2, 6);   // bank vertical padding    (was fixed 6)
    const int modGap       = lerpPad (hs, 2, 6);   // gap between modules      (was fixed 6)
    const int sectionPad   = lerpPad (ws, 2, 8);   // amp/filter section pad   (was fixed 8)

    auto bounds = getLocalBounds().reduced (outerPadH, outerPadV);
    constexpr int topBarHeight = 84;
    auto topBarArea = bounds.removeFromTop (topBarHeight);
    topBar->setBounds (topBarArea);
    bounds.removeFromTop (topGap);
    auto topArea = bounds;

    auto topRow = topArea;
    const auto columnWidth = (topRow.getWidth() - (colGap * 2)) / 3;
    auto leftBankArea = topRow.removeFromLeft (columnWidth);
    topRow.removeFromLeft (colGap);
    auto centerArea = topRow.removeFromLeft (columnWidth);
    topRow.removeFromLeft (colGap);
    auto rightBankArea = topRow;

    const int bottomStripHeight = juce::roundToInt (static_cast<float> (topArea.getHeight()) * 0.35f);
    auto bottomLeftArea = leftBankArea.removeFromBottom (bottomStripHeight);
    auto bottomRightArea = rightBankArea.removeFromBottom (bottomStripHeight);

    auto layoutBankModules = [bankPadH, bankPadV, modGap] (
        juce::Rectangle<int> bankBounds,
        std::array<std::unique_ptr<OscillatorModuleComponent>, modulesPerBank>& modules)
    {
        bankBounds = bankBounds.reduced (bankPadH, bankPadV);
        const auto moduleHeight = (bankBounds.getHeight() - (modGap * (modulesPerBank - 1))) / modulesPerBank;

        for (int index = 0; index < modulesPerBank; ++index)
        {
            auto moduleBounds = bankBounds.removeFromTop (moduleHeight);
            if (index < modulesPerBank - 1)
                bankBounds.removeFromTop (modGap);

            modules[static_cast<size_t> (index)]->setBounds (moduleBounds);
        }
    };

    layoutBankModules (leftBankArea, leftModules);
    layoutBankModules (rightBankArea, rightModules);

    ampSection->setBounds (bottomLeftArea.reduced (sectionPad));
    filterSection->setBounds (bottomRightArea.reduced (sectionPad));

    auto centerColumn = centerArea.reduced (bankPadH, bankPadV);
    const auto viewportHeight = juce::roundToInt (static_cast<float> (centerColumn.getHeight()) * 0.6f);
    glViewport_.setBounds (centerColumn.removeFromTop (viewportHeight));
    centerPanel->setBounds (centerColumn);
}

void SynthEditorWorkspace::tick()
{
    auto& apvts = processorRef.getParameterManager().getAPVTS();

    processorRef.setGuiCubeRotation (renderer3D_.getCubeRotationQuat());

    auto* px = apvts.getRawParameterValue (ParameterIDs::cursorX);
    auto* py = apvts.getRawParameterValue (ParameterIDs::cursorY);
    auto* pz = apvts.getRawParameterValue (ParameterIDs::cursorZ);
    const glm::vec3 uFromApvts (
        px != nullptr ? px->load() : 0.5f,
        py != nullptr ? py->load() : 0.5f,
        pz != nullptr ? pz->load() : 0.5f);

    const bool spinActive = renderer3D_.isSpinActive();

    // Slider / joystick / host automation: always push into the GL view and atomic state, even
    // while the cube is spinning. Otherwise `isSpinActive()` blocks this path and controls feel dead.
    if (glm::distance (uFromApvts, lastSyncedCursorGlobal_) > 1.0e-4f)
    {
        lastSyncedCursorGlobal_ = uFromApvts;
        renderer3D_.setCursorFromGlobalUnitPosition (uFromApvts);
        processorRef.setGuiCursorPosition (uFromApvts);
    }
    else
    {
        const glm::vec3 uFromView = renderer3D_.getCursorAsGlobalUnitPosition();
        if (glm::distance (uFromView, lastSyncedCursorGlobal_) > 1.0e-4f)
        {
            lastSyncedCursorGlobal_ = uFromView;
            processorRef.setGuiCursorPosition (uFromView);
            const bool shouldWriteParams = ! spinActive
                                           || (renderer3D_.hasActiveDrag() && ! renderer3D_.isAltSpinDragging());
            if (shouldWriteParams)
                updateCursorParametersFromPosition (uFromView);
        }
    }

    const auto* zeroGParam = apvts.getRawParameterValue (ParameterIDs::cubeZeroG);
    const bool zeroGEnabled = (zeroGParam != nullptr) && (zeroGParam->load() > 0.5f);
    renderer3D_.setZeroGravity (zeroGEnabled);

    if (const auto* zoomParam = apvts.getRawParameterValue (ParameterIDs::cameraZoom))
        renderer3D_.setCameraZoom (zoomParam->load());

    if (const auto* gizmoParam = apvts.getRawParameterValue (ParameterIDs::gizmoVisible))
        renderer3D_.setGizmoVisible (gizmoParam->load() > 0.5f);

    topBar->setMeterLevels (
        processorRef.getSynthEngine().getMeterLevelLeft(),
        processorRef.getSynthEngine().getMeterLevelRight());
}

void SynthEditorWorkspace::updateCursorParametersFromPosition (glm::vec3 position)
{
    auto& apvts = processorRef.getParameterManager().getAPVTS();
    if (auto* px = apvts.getParameter (ParameterIDs::cursorX))
        px->setValueNotifyingHost (position.x);
    if (auto* py = apvts.getParameter (ParameterIDs::cursorY))
        py->setValueNotifyingHost (position.y);
    if (auto* pz = apvts.getParameter (ParameterIDs::cursorZ))
        pz->setValueNotifyingHost (position.z);
}

void SynthEditorWorkspace::mouseDown (const juce::MouseEvent& event)
{
    const auto vpBounds = glViewport_.getBounds();
    if (vpBounds.contains (event.getPosition()))
    {
        renderer3D_.mouseDown (event, vpBounds);
        const bool shiftDown = event.mods.isShiftDown()
                               || juce::ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
        const bool altDown = event.mods.isAltDown()
                             || juce::ModifierKeys::getCurrentModifiersRealtime().isAltDown();
        if (! shiftDown && ! altDown)
        {
            const auto newCursor = renderer3D_.getCursorAsGlobalUnitPosition ();
            processorRef.setGuiCursorPosition (newCursor);
            updateCursorParametersFromPosition (newCursor);
            lastSyncedCursorGlobal_ = newCursor;
        }
    }
    else
    {
        cancelViewportInteraction();
    }
}

void SynthEditorWorkspace::mouseDrag (const juce::MouseEvent& event)
{
    const auto vpBounds = glViewport_.getBounds();
    if (vpBounds.contains (event.getPosition()))
    {
        renderer3D_.mouseDrag (event, vpBounds);

        const auto newCursor = renderer3D_.getCursorAsGlobalUnitPosition ();
        processorRef.setGuiCursorPosition (newCursor);
        const bool altDown = event.mods.isAltDown()
                             || juce::ModifierKeys::getCurrentModifiersRealtime().isAltDown();
        if (! altDown)
            updateCursorParametersFromPosition (newCursor);
        lastSyncedCursorGlobal_ = newCursor;
    }
}

void SynthEditorWorkspace::mouseUp (const juce::MouseEvent& event)
{
    renderer3D_.mouseUp (event);
}

void SynthEditorWorkspace::mouseWheelMove (const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (glViewport_.getBounds().contains (event.getPosition()))
        renderer3D_.mouseWheelMove (event, wheel);
}

void SynthEditorWorkspace::cancelViewportInteraction()
{
    renderer3D_.cancelInteraction();
}

} // namespace UI
