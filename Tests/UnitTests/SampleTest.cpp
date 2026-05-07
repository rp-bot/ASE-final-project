/*
  ==============================================================================

    SampleTest.cpp
    Created: 13 Feb 2026
    Description: A sample unit test.

  ==============================================================================
*/

#include <JuceHeader.h>

class SampleTest : public juce::UnitTest
{
public:
    SampleTest() : juce::UnitTest ("Sample Test") {}

    void runTest() override
    {
        beginTest ("Basic Arithmetic");
        expect (1 + 1 == 2, "1 + 1 should equal 2");

        beginTest ("String Comparison");
        expect (juce::String ("Hello") == "Hello", "Strings should match");
    }
};

static SampleTest sampleTest;
