#include "VolumetricSynthAudioProcessor.h"
#include "VolumetricSynthEditor.h"
#include "../Parameters/ParameterIDs.h"
#include "../Audio/SynthEngine.h"

//==============================================================================
VolumetricSynthEditor::VolumetricSynthEditor(VolumetricSynthAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    auto &apvts = processorRef.getParameterManager().getAPVTS();
    const std::array<juce::Colour, 8> moduleColours{
        juce::Colour::fromRGB(231, 76, 60),  // red
        juce::Colour::fromRGB(230, 126, 34), // orange
        juce::Colour::fromRGB(241, 196, 15), // yellow
        juce::Colour::fromRGB(46, 204, 113), // green
        juce::Colour::fromRGB(26, 188, 156), // teal
        juce::Colour::fromRGB(52, 152, 219), // blue
        juce::Colour::fromRGB(155, 89, 182), // purple
        juce::Colour::fromRGB(236, 112, 173) // pink
    };

    for (int module = 0; module < modulesPerBank; ++module)
    {
        leftModules[static_cast<size_t>(module)] = std::make_unique<UI::OscillatorModuleComponent>(
            apvts,
            module,
            "Osc " + juce::String(module + 1),
            moduleColours[static_cast<size_t>(module)]);
        addAndMakeVisible(*leftModules[static_cast<size_t>(module)]);

        rightModules[static_cast<size_t>(module)] = std::make_unique<UI::OscillatorModuleComponent>(
            apvts,
            modulesPerBank + module,
            "Osc " + juce::String(modulesPerBank + module + 1),
            moduleColours[static_cast<size_t>(modulesPerBank + module)]);
        addAndMakeVisible(*rightModules[static_cast<size_t>(module)]);
    }

    centerPanel = std::make_unique<UI::CenterControlPanel>(apvts);
    addAndMakeVisible(*centerPanel);

    // addAndMakeVisible (bottomLeftPanel);

    masterControlSection = std::make_unique<UI::MasterControls> (apvts);
    addAndMakeVisible(*masterControlSection);

    outputSection = std::make_unique<UI::OutputSection>(apvts, [this]()
                                                         { processorRef.resetEngineHardOff(); });
    addAndMakeVisible(*outputSection);

    addAndMakeVisible(glViewport_);
    glContextHost_.setRenderer(&renderer3D_);
    glContextHost_.attachTo(glViewport_);

    {
        std::array<glm::vec4, 8> cornerColours{};
        for (size_t i = 0; i < cornerColours.size(); ++i)
        {
            const auto c = moduleColours[i];
            cornerColours[i] = {c.getFloatRed(), c.getFloatGreen(), c.getFloatBlue(), 1.0f};
        }
        renderer3D_.setCornerColours(cornerColours);
    }

    renderer3D_.setCursorFromUnitPosition(processorRef.getGuiCursorPosition());

    startTimerHz(25);

    setSize (1220, 800);
}

VolumetricSynthEditor::~VolumetricSynthEditor()
{
    glContextHost_.detach();
}

//==============================================================================
void VolumetricSynthEditor::timerCallback()
{
    renderer3D_.setCursorFromUnitPosition(processorRef.getGuiCursorPosition());

    outputSection->setMeterLevels(
        processorRef.getSynthEngine().getMeterLevelLeft(),
        processorRef.getSynthEngine().getMeterLevelRight());

    //TODO: LINK PARAMS
}

void VolumetricSynthEditor::updateCursorParametersFromPosition(glm::vec3 position)
{
    auto &apvts = processorRef.getParameterManager().getAPVTS();
    if (auto *px = apvts.getParameter(ParameterIDs::cursorX))
        px->setValueNotifyingHost(position.x);
    if (auto *py = apvts.getParameter(ParameterIDs::cursorY))
        py->setValueNotifyingHost(position.y);
    if (auto *pz = apvts.getParameter(ParameterIDs::cursorZ))
        pz->setValueNotifyingHost(position.z);
}

void VolumetricSynthEditor::paint(juce::Graphics &g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::grey.withAlpha(0.6f));
    g.drawRect(leftBankArea, 1);
    g.drawRect(rightBankArea, 1);
    g.drawRect(centerArea, 1);

    g.setColour(juce::Colours::white);
    g.setFont(16.0f);

    g.drawText("Volumetric Synth", getLocalBounds().removeFromTop(24), juce::Justification::centred, false);
}

void VolumetricSynthEditor::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    topArea = bounds;

    auto topRow = topArea;
    constexpr int columnGap = 8;
    const auto columnWidth = (topRow.getWidth() - (columnGap * 2)) / 3;
    leftBankArea = topRow.removeFromLeft(columnWidth);
    topRow.removeFromLeft(columnGap);
    centerArea = topRow.removeFromLeft(columnWidth);
    topRow.removeFromLeft(columnGap);
    rightBankArea = topRow;

    const int bottomStripHeight = juce::roundToInt (static_cast<float> (topArea.getHeight()) * 0.35f);
    bottomLeftArea = leftBankArea.removeFromBottom (bottomStripHeight);
    bottomRightArea = rightBankArea.removeFromBottom (bottomStripHeight);

    auto layoutBankModules = [](juce::Rectangle<int> bankBounds,
                                std::array<std::unique_ptr<UI::OscillatorModuleComponent>, modulesPerBank> &modules)
    {
        bankBounds = bankBounds.reduced(6);
        constexpr int moduleGap = 6;
        const auto moduleHeight = (bankBounds.getHeight() - (moduleGap * (modulesPerBank - 1))) / modulesPerBank;

        for (int index = 0; index < modulesPerBank; ++index)
        {
            auto moduleBounds = bankBounds.removeFromTop(moduleHeight);
            if (index < modulesPerBank - 1)
                bankBounds.removeFromTop(moduleGap);

            modules[static_cast<size_t>(index)]->setBounds(moduleBounds);
        }
    };

    layoutBankModules(leftBankArea, leftModules);
    layoutBankModules(rightBankArea, rightModules);

    // bottomLeftPanel.setBounds (bottomLeftArea.reduced (8));
    masterControlSection->setBounds(bottomLeftArea.reduced(8));
    outputSection->setBounds (bottomRightArea.reduced (8));

    auto centerColumn = centerArea.reduced(6);
    const auto viewportHeight = juce::roundToInt(static_cast<float>(centerColumn.getHeight()) * 0.6f);
    glViewport_.setBounds(centerColumn.removeFromTop(viewportHeight));
    centerPanel->setBounds(centerColumn);
}

void VolumetricSynthEditor::mouseDown(const juce::MouseEvent &event)
{
    const auto vpBounds = glViewport_.getBounds();
    if (vpBounds.contains(event.getPosition()))
    {
        renderer3D_.mouseDown(event, vpBounds);
        if (!event.mods.isShiftDown())
        {
            const auto newCursor = renderer3D_.getCursorAsUnitPosition();
            processorRef.setGuiCursorPosition(newCursor);
            updateCursorParametersFromPosition(newCursor);
        }
    }
}

void VolumetricSynthEditor::mouseDrag(const juce::MouseEvent &event)
{
    const auto vpBounds = glViewport_.getBounds();
    if (vpBounds.contains(event.getPosition()))
    {
        renderer3D_.mouseDrag(event, vpBounds);

        const auto newCursor = renderer3D_.getCursorAsUnitPosition();
        processorRef.setGuiCursorPosition(newCursor);
        updateCursorParametersFromPosition(newCursor);
    }
}

void VolumetricSynthEditor::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel)
{
    if (glViewport_.getBounds().contains(event.getPosition()))
        renderer3D_.mouseWheelMove(event, wheel);
}
