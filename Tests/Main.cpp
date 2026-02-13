/*
  ==============================================================================

    Main.cpp
    Created: 13 Feb 2026
    Description: Entry point for the unit test runner.

  ==============================================================================
*/

#include <JuceHeader.h>

int main (int argc, char* argv[])
{
    juce::ignoreUnused (argc, argv);

    juce::UnitTestRunner runner;
    runner.runAllTests();

    return 0;
}
