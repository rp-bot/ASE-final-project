#include "SynthSound.h"

namespace Audio 
{

bool SynthSound::appliesToNote(int midiNoteNumber) {
    return midiNoteNumber >= 0 && midiNoteNumber <= 127;
}

// returns true for channels the synth responds to. Leaving as all for now
bool SynthSound::appliesToChannel(int midiChannel) { 
    return true;
}

}