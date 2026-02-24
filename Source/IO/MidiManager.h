#pragma once

// #include <juce_audio_devices/midi_io>
#include "juce_audio_devices/midi_io/juce_MidiDevices.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <string>
#include <vector>
#include <JuceHeader.h>
#include <functional>

struct MidiEvent
{
    enum class Type { NoteOn, NoteOff, Other };
    //TODO: ADD TO MIDI EVENT DEFINITION
};



class MidiManager : private juce::MidiInputCallback
{
public:

    MidiManager() = default;
    ~MidiManager() { closeMIDI(); }

    // get input device names
    // returns: vector of all input device names
    std::vector<std::string> getInputDeviceNames() const; //const: ensures the midi devices not modified

    // opens device at specified deviceIndex based on avaliable devices list
    // returns: bool (status for success)
    bool openInput(int deviceIndex);

    // closes saved midi device & set to null
    void closeMIDI();

private:
    std::unique_ptr<juce::MidiInput> midiInput = nullptr;
};