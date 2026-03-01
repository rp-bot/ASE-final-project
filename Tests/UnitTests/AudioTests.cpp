#include <JuceHeader.h>
#include "Audio/SynthSound.h"

class SynthSoundTest : public juce::UnitTest
{
public:
    SynthSoundTest() : juce::UnitTest ("Synth Sound Test") {}

    void runTest() override
    {
        Audio::SynthSound testSound;
        beginTest ("AppliesToNote");
        expect (testSound.appliesToNote(0), "midi note 0 should be in range");
        expect (testSound.appliesToNote(60), "midi note 60 should be in range");
        expect (testSound.appliesToNote(127), "midi note 127 should be in range");
        expect (testSound.appliesToNote(128) == 0, "midi note 128 should be out of range");

        beginTest ("AppliesToChannel");
        expect (testSound.appliesToChannel(1), "chanel 1");
    }
};

static SynthSoundTest SynthSoundTest;