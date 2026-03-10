/*
  ==============================================================================

    AssertionsTest.cpp
    Unit tests for Utils assertion macros (VOLUMETRIC_ASSERT, VOLUMETRIC_ASSERT_MSG,
    VOLUMETRIC_RT_ASSERT). Release: macros compile to nothing and do not evaluate
    conditions. Debug: VOLUMETRIC_ASSERT(false) would trigger; not run in automated tests.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Utils/Assertions.h"

class AssertionsTest : public juce::UnitTest
{
public:
    AssertionsTest() : juce::UnitTest("Assertions", "Utils") {}

    void runTest() override
    {
#if JUCE_DEBUG
        runDebugAssertPassTest();
#endif
        runReleaseNoOpTest();
        runConditionNotEvaluatedInReleaseTest();
    }

private:
#if JUCE_DEBUG
    void runDebugAssertPassTest()
    {
        beginTest("Debug: VOLUMETRIC_ASSERT(true) and VOLUMETRIC_RT_ASSERT(true) do not trigger");
        VOLUMETRIC_ASSERT(true);
        VOLUMETRIC_RT_ASSERT(true);
        VOLUMETRIC_ASSERT_MSG(true, "passing case");
    }
#endif

    void runReleaseNoOpTest()
    {
#if !JUCE_DEBUG
        beginTest("Release: VOLUMETRIC_ASSERT(false) and VOLUMETRIC_RT_ASSERT(false) compile to nothing");
        VOLUMETRIC_ASSERT(false);
        VOLUMETRIC_RT_ASSERT(false);
        VOLUMETRIC_ASSERT_MSG(false, "should not run");
        // If we get here without aborting, the test passes.
#else
        beginTest("Release no-op test (only runs in Release build)");
#endif
    }

    void runConditionNotEvaluatedInReleaseTest()
    {
#if !JUCE_DEBUG
        beginTest("Release: condition is not evaluated when assertions are off");
        static int counter = 0;
        counter = 0;
        VOLUMETRIC_ASSERT(++counter == 0);
        expect(counter == 0, "Condition must not be evaluated in Release");
#else
        beginTest("Release condition-not-evaluated test (only runs in Release build)");
#endif
    }
};

static AssertionsTest assertionsTest;
