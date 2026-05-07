/*
  ==============================================================================

    ScopedDenormalsTest.cpp
    Unit tests for Utils::ScopedDenormals (RAII denormal disable/restore).
    Lifetime: FP mode is restored after scope exit (x86). No-crash: many create/destroy.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Utils/ScopedDenormals.h"

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    #include <xmmintrin.h>
    #define SCOPE_DENORMALS_TEST_X86 1
#endif

class ScopedDenormalsTest : public juce::UnitTest
{
public:
    ScopedDenormalsTest() : juce::UnitTest("ScopedDenormals", "Utils") {}

    void runTest() override
    {
        runLifetimeRestoreTest();
        runNoCrashTest();
    }

private:
    void runLifetimeRestoreTest()
    {
#if defined(SCOPE_DENORMALS_TEST_X86)
        beginTest("Lifetime: FP mode restored after scope exit (x86)");
        unsigned int stateBefore = static_cast<unsigned int>(_mm_getcsr());
        {
            Utils::ScopedDenormals scope;
            unsigned int stateInside = static_cast<unsigned int>(_mm_getcsr());
            expect((stateInside & 0x8040u) == 0x8040u, "Inside scope, FTZ and DAZ should be set");
        }
        unsigned int stateAfter = static_cast<unsigned int>(_mm_getcsr());
        expect(stateAfter == stateBefore, "After scope exit, MXCSR should be restored to original");
#else
        beginTest("Lifetime: no-op on non-x86 (no crash)");
        {
            Utils::ScopedDenormals scope;
        }
#endif
    }

    void runNoCrashTest()
    {
        beginTest("No crash: many create/destroy");
        for (int i = 0; i < 1000; ++i)
        {
            Utils::ScopedDenormals scope;
        }
    }
};

static ScopedDenormalsTest scopedDenormalsTest;
