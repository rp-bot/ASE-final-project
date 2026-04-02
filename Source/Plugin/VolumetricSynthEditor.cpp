#include "VolumetricSynthAudioProcessor.h"
#include "VolumetricSynthEditor.h"

//==============================================================================
VolumetricSynthEditor::VolumetricSynthEditor (VolumetricSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    auto& apvts = processorRef.getParameterManager().getAPVTS();
    const std::array<juce::Colour, 8> moduleColours
    {
        juce::Colour::fromRGB (231, 76, 60),   // red
        juce::Colour::fromRGB (230, 126, 34),  // orange
        juce::Colour::fromRGB (241, 196, 15),  // yellow
        juce::Colour::fromRGB (46, 204, 113),  // green
        juce::Colour::fromRGB (26, 188, 156),  // teal
        juce::Colour::fromRGB (52, 152, 219),  // blue
        juce::Colour::fromRGB (155, 89, 182),  // purple
        juce::Colour::fromRGB (236, 112, 173)  // pink
    };

    for (int module = 0; module < modulesPerBank; ++module)
    {
        leftModules[static_cast<size_t> (module)] = std::make_unique<UI::OscillatorModuleComponent> (
            apvts,
            module,
            "Osc " + juce::String (module + 1),
            moduleColours[static_cast<size_t> (module)]);
        addAndMakeVisible (*leftModules[static_cast<size_t> (module)]);

        rightModules[static_cast<size_t> (module)] = std::make_unique<UI::OscillatorModuleComponent> (
            apvts,
            modulesPerBank + module,
            "Osc " + juce::String (modulesPerBank + module + 1),
            moduleColours[static_cast<size_t> (modulesPerBank + module)]);
        addAndMakeVisible (*rightModules[static_cast<size_t> (module)]);
    }

    centerPanel = std::make_unique<UI::CenterControlPanel> (apvts);
    centerPanel->setTrajectoryChangedCallback ([this] (bool isActive)
    {
        processorRef.setGuiTrajectoryActive (isActive);
    });
    addAndMakeVisible (*centerPanel);
    addAndMakeVisible (bottomLeftPanel);
    addAndMakeVisible (bottomCenterPanel);
    addAndMakeVisible (bottomRightPanel);

    centerPanel->setTrajectoryActive (processorRef.isGuiTrajectoryActive());

    setSize (1220, 700);
}

VolumetricSynthEditor::~VolumetricSynthEditor()
{
}

//==============================================================================
void VolumetricSynthEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::grey.withAlpha (0.6f));
    g.drawRect (leftBankArea, 1);
    g.drawRect (rightBankArea, 1);
    g.drawRect (centerArea, 1);

    g.setColour (juce::Colours::white);
    g.setFont (16.0f);

    // g.drawText ("Volumetric Synth", getLocalBounds().removeFromTop (24), juce::Justification::centred, false);
}

void VolumetricSynthEditor::resized()
{
    auto bounds = getLocalBounds().reduced (8);
    topArea = bounds.removeFromTop (juce::roundToInt (bounds.getHeight() * 0.72f));
    bottomArea = bounds;

    auto topRow = topArea;
    constexpr int columnGap = 8;
    const auto columnWidth = (topRow.getWidth() - (columnGap * 2)) / 3;
    leftBankArea = topRow.removeFromLeft (columnWidth);
    topRow.removeFromLeft (columnGap);
    centerArea = topRow.removeFromLeft (columnWidth);
    topRow.removeFromLeft (columnGap);
    rightBankArea = topRow;

    auto layoutBankModules = [] (juce::Rectangle<int> bankBounds,
                                 std::array<std::unique_ptr<UI::OscillatorModuleComponent>, modulesPerBank>& modules)
    {
        bankBounds = bankBounds.reduced (6);
        constexpr int moduleGap = 6;
        const auto moduleHeight = (bankBounds.getHeight() - (moduleGap * (modulesPerBank - 1))) / modulesPerBank;

        for (int index = 0; index < modulesPerBank; ++index)
        {
            auto moduleBounds = bankBounds.removeFromTop (moduleHeight);
            if (index < modulesPerBank - 1)
                bankBounds.removeFromTop (moduleGap);

            modules[static_cast<size_t> (index)]->setBounds (moduleBounds);
        }
    };

    layoutBankModules (leftBankArea, leftModules);
    layoutBankModules (rightBankArea, rightModules);
    centerPanel->setBounds (centerArea.reduced (6));

    auto bottomRow = bottomArea;
    const auto bottomColumnWidth = (bottomRow.getWidth() - (columnGap * 2)) / 3;
    bottomLeftArea = bottomRow.removeFromLeft (bottomColumnWidth);
    bottomRow.removeFromLeft (columnGap);
    bottomCenterArea = bottomRow.removeFromLeft (bottomColumnWidth);
    bottomRow.removeFromLeft (columnGap);
    bottomRightArea = bottomRow;

    bottomLeftPanel.setBounds (bottomLeftArea.reduced (8));
    bottomCenterPanel.setBounds (bottomCenterArea.reduced (8));
    bottomRightPanel.setBounds (bottomRightArea.reduced (8));
}
